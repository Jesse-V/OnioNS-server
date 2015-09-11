
#include "Session.hpp"
#include "../Mirror.hpp"
#include <onions-common/Common.hpp>
#include <onions-common/containers/Cache.hpp>
#include <onions-common/Log.hpp>
#include <onions-common/Utils.hpp>
#include <onions-common/tcp/MemAllocator.hpp>
#include <botan/sha2_64.h>
#include <botan/base64.h>
#include <boost/bind.hpp>


template <typename Handler>
inline MemAllocator<Handler> makeHandler(HandleAlloc& a, Handler h)
{
  return MemAllocator<Handler>(a, h);
}



Session::Session(const SocketPtr& socket, int id) : socket_(socket), id_(id)
{
}



Session::~Session()
{
  Log::get().notice("Session " + std::to_string(id_) + " deallocating.");
}



void Session::asyncRead()
{
  socket_->async_read_some(
      boost::asio::buffer(inChunk_),
      makeHandler(allocator_,
                  boost::bind(&Session::processRead, shared_from_this(),
                              boost::asio::placeholders::error,
                              boost::asio::placeholders::bytes_transferred)));
}



void Session::asyncWrite(const std::string& type, const std::string& val)
{
  Json::Value out;
  out["type"] = type;
  out["value"] = val;
  asyncWrite(out);
}



void Session::asyncWrite(const Json::Value& val)
{
  Json::FastWriter writer;
  asyncWrite(writer.write(val));
}



// ***************************** PRIVATE METHODS *****************************



Json::Value Session::respond(const std::string& inputStr)
{
  Json::Value in, out;
  Json::Reader reader;
  out["type"] = "error";
  out["value"] = "";

  if (!reader.parse(inputStr, in))
    out["value"] = "Failed to parse message!";
  else if (!in.isMember("type"))
    out["value"] = "Message is missing the \"type\" field!";
  else if (!in.isMember("value"))
    out["value"] = "Message is missing the \"value\" field!";
  else
  {
    std::string type(in["type"].asString());
    Log::get().notice(std::to_string(id_) + ": Received " + type);

    if (type == "upload")
      out = respondToUpload(in);
    else if (type == "domainQuery")
      out = respondToDomainQuery(in);
    else if (type == "subscribe")
      out = respondToSubscribe(in);
    else if (type == "merkleSignature")
      out = respondToMerkleSignature(in);
    else if (type == "SYN")
    {
      out["type"] = "success";
      out["value"] = "ACK";
    }
    else if (type == "success")
    {  // no need to reply
      Log::get().notice(std::to_string(id_) + ": Response: \"" +
                        in["value"].asString() + "\"");
      return nullptr;
    }
    else
    {
      out["type"] = "error";
      out["value"] = "Unknown type \"" + type + "\"";
    }
  }

  if (out["type"].asString() == "error")
  {
    Log::get().warn(std::to_string(id_) + ":     \"" + inputStr + "\"");
    Log::get().warn(std::to_string(id_) + ": " + out["value"].asString());
  }

  return out;
}



Json::Value Session::respondToUpload(const Json::Value& in)
{
  Json::Value response;
  auto r = Common::parseRecord(in["value"].asString());
  Log::get().notice(std::to_string(id_) + ": received a Record for \"" +
                    r->getName() + "\"");

  if (Mirror::processNewRecord(r))
  {
    Log::get().notice(std::to_string(id_) + ": Record successfully processed.");
    response["type"] = "success";
    response["value"] = "";
  }
  else
  {
    response["type"] = "error";
    response["value"] = "Name-already-taken.";
  }

  return response;
}



Json::Value Session::respondToDomainQuery(const Json::Value& in)
{
  Json::Value response;
  std::string domain = in["value"].asString();
  if (Utils::strEndsWith(domain, ".tor"))
  {  // resolve .tor -> .onion

    auto record = Cache::get(domain);
    if (record)
    {
      response["type"] = "success";
      response["value"] = record->asJSON();
      Log::get().notice(std::to_string(id_) + ": Found Record for \"" + domain +
                        "\"");
    }
    else
    {
      response["type"] = "error";
      response["value"] = "404";
      Log::get().notice(std::to_string(id_) + ": 404ed request for \"" +
                        domain + "\"");
    }
  }
  else
  {
    response["type"] = "error";
    response["value"] = "Invalid request.";
  }

  return response;
}



Json::Value Session::respondToSubscribe(const Json::Value& in)
{
  Log::get().notice(std::to_string(id_) + " has subscribed.");

  Json::Value response;
  Mirror::addSubscriber(boost::shared_ptr<Session>(this));
  response["type"] = "success";
  response["value"] = "";
  return response;
}



Json::Value Session::respondToMerkleSignature(const Json::Value& in)
{
  Log::get().notice(std::to_string(id_) + " received Merkle tree signature.");

  // todo
  Json::Value response;
  response["type"] = "success";
  response["value"] = "";
  return response;
}



// called by Asio whenever the socket has been read into the buffer
void Session::processRead(const boost::system::error_code& error, size_t n)
{
  // check error
  if (n == 0)
  {
    Log::get().notice(std::to_string(id_) + ": Connection closed.");
    return;
  }
  else if (error)
  {
    Log::get().warn(std::to_string(id_) + ": " + error.message());
    return;
  }

  // add to buffer
  inBuffer_.append(std::string(inChunk_.begin(), inChunk_.begin() + n));

  // check if we have the whole message (#64)
  std::size_t delimiter = inBuffer_.find_first_of('\n');
  if (delimiter == std::string::npos)
  {  // the whole message isn't here yet, so read more asynchronously
    asyncRead();
  }
  else
  {
    // cut out word
    std::string input = inBuffer_.substr(0, delimiter);
    inBuffer_.erase(0, delimiter);

    // process message
    auto response = respond(input);
    if (response == nullptr)
      asyncRead();  // no reply need, so read again
    else
      asyncWrite(response);
  }
}



// called by Asio when the buffer has been written to the socket
// we pass the buffer argument so that this is the end of its lifetime
void Session::processWrite(const boost::system::error_code& error,
                           const std::shared_ptr<Buffer>& buffer)
{
  if (error)
  {
    Log::get().warn(std::to_string(id_) + ": " + error.message());
    return;
  }

  asyncRead();
}



void Session::asyncWrite(const std::string& str)
{
  auto writeBuffer = std::make_shared<Buffer>();
  std::copy(str.begin(), str.end(), writeBuffer->data());

  boost::asio::async_write(
      *socket_, boost::asio::buffer(*writeBuffer, str.size()),
      makeHandler(allocator_,
                  boost::bind(&Session::processWrite, shared_from_this(),
                              boost::asio::placeholders::error, writeBuffer)));
}
