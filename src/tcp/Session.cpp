
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
  Mirror::get().removeSubscriber(this);
}



int Session::getID() const
{
  return id_;
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



void Session::processMessage(const std::string& inputStr)
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
  else if (!respond(in, out))
  {
    asyncRead();  // no reply need, so read again
    return;
  }

  if (out["type"].asString() == "error")
  {
    Log::get().warn(std::to_string(id_) + ":     \"" + inputStr + "\"");
    Log::get().warn(std::to_string(id_) + ": " + out["value"].asString());
  }

  out["signature"] = Mirror::get().signTransmission(out);
  asyncWrite(out);
}



bool Session::respond(const Json::Value& in, Json::Value& out)
{
  std::string type(in["type"].asString());
  Log::get().notice(std::to_string(id_) + ": Received " + type);

  if (type == "putRecord")
    out = respondToPutRecord(in);
  else if (type == "domainQuery")
    out = respondToDomainQuery(in);
  else if (type == "getMerkleSubtree")
    out = respondToGetMerkleSubtree(in);
  else if (type == "getRootSignature")
    out = Mirror::get().getRootSignature();
  else if (type == "SYN")
  {
    out["type"] = "success";
    out["value"] = "ACK";
  }
  else if (type == "waitForRecord")
  {
    Mirror::get().addSubscriber(this);
    Log::get().notice(std::to_string(id_) + ": is waiting for Records.");
    return false;
  }
  else if (type == "success")
    return false;
  else
  {  // todo: #78 might still be here
    out["type"] = "error";
    out["value"] = "Unknown type \"" + type + "\"";
  }

  return true;
}



Json::Value Session::respondToPutRecord(const Json::Value& in) const
{
  Json::Value response;
  auto r = Common::parseRecord(in["value"].asString());
  Log::get().notice(std::to_string(id_) + ": received a Record for \"" +
                    r->getName() + "\"");

  if (Mirror::get().processNewRecord(id_, r))
  {
    Log::get().notice(std::to_string(id_) + ": Record successfully processed.");
    response["type"] = "success";
    response["value"] = "";
  }
  else
  {
    response["type"] = "error";
    response["value"] = "Name already taken.";
  }

  return response;
}



Json::Value Session::respondToDomainQuery(const Json::Value& in) const
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



Json::Value Session::respondToGetMerkleSubtree(const Json::Value& in) const
{
  Json::Value response;

  std::string domain = in["value"].asString();
  if (Utils::strEndsWith(domain, ".tor"))
  {
    response["type"] = "success";
    response["value"] = Mirror::get().getMerkleTree()->getPathTo(domain);
    Log::get().notice(std::to_string(id_) + ": Assembled subtree for \"" +
                      domain + "\"");
  }
  else
  {
    response["type"] = "error";
    response["value"] = "Invalid request.";
    Log::get().notice(std::to_string(id_) + ": Failed subtree request for \"" +
                      domain + "\"");
  }

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
    inBuffer_.erase(0, delimiter + 1);
    processMessage(input);
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
