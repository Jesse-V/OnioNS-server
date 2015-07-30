
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



Session::Session(boost::asio::io_service& ios, int id)
    : socket_(ios), subscribed_(false), id_(id)
{
}



Session::~Session()
{
  std::cout << "Session " << id_ << " deallocating." << std::endl;
}



boost::asio::ip::tcp::socket& Session::getSocket()
{
  return socket_;
}



void Session::asyncRead()
{
  socket_.async_read_some(
      boost::asio::buffer(buffer_),
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



bool Session::isSubscriber()
{
  return subscribed_;
}



// ***************************** PRIVATE METHODS *****************************



void Session::handlePing(Json::Value& in, Json::Value& out)
{
  out["value"] = "pong";
}



void Session::handleUpload(Json::Value& in, Json::Value& out)
{
  auto r = Common::parseRecord(in["value"].asString());
  if (Cache::add(r))  // if successfully added to the Cache
  {
    Log::get().notice(std::to_string(id_) +
                      ": Cached new Record. Broadcasting...");
    Mirror::broadcastEvent("upload", in["value"]);
    Log::get().notice(std::to_string(id_) + ": Finished broadcasting Record.");

    out["value"] = "success";
  }
  else
    out["error"] = "Name already taken.";
}



void Session::handleDomainQuery(Json::Value& in, Json::Value& out)
{
  std::string domain = in["value"].asString();
  if (Utils::strEndsWith(domain, ".tor"))
  {  // resolve .tor -> .onion

    auto record = Cache::get(domain);
    if (record)
    {
      out["value"] = record->asJSON();
      Log::get().notice(std::to_string(id_) + ": Found Record for \"" + domain +
                        "\"");
    }
    else
    {
      out["error"] = "404";
      Log::get().notice(std::to_string(id_) + ": 404ed request for \"" +
                        domain + "\"");
    }
  }
  else
    out["error"] = "Invalid request.";
}



void Session::handleSubscribe(Json::Value& in, Json::Value& out)
{
  Log::get().notice(std::to_string(id_) + " has subscribed.");
  subscribed_ = true;
  out["value"] = "success";
}



// called by Asio whenever the socket has been read into the buffer
void Session::processRead(const boost::system::error_code& error, size_t n)
{
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

  Json::Value in, out;
  Json::Reader reader;
  std::string inputStr(buffer_.begin(), buffer_.begin() + n);
  out["type"] = "response";
  out["value"] = "";

  if (!reader.parse(inputStr, in))
    out["error"] = "Failed to parse message!";
  else if (!in.isMember("type"))
    out["error"] = "Message is missing the \"type\" field!";
  else if (!in.isMember("value"))
    out["error"] = "Message is missing the \"value\" field!";
  else
  {
    std::string type(in["type"].asString());

    Log::get().notice(std::to_string(id_) + ": Received " + type);

    if (type == "ping")
      handlePing(in, out);
    else if (type == "upload")
      handleUpload(in, out);
    else if (type == "domainQuery")
      handleDomainQuery(in, out);
    else if (type == "subscribe")
      handleSubscribe(in, out);
    else if (type == "response")
    {
      Log::get().notice(std::to_string(id_) + ": Response: \"" +
                        in["value"].asString() + "\"");
      asyncRead();  // no reply necessary
      return;
    }
    else
      out["error"] = "Unknown type \"" + type + "\"";
  }

  if (out.isMember("error"))
  {
    Log::get().warn(std::to_string(id_) + ":     \"" + inputStr + "\"");
    Log::get().warn(std::to_string(id_) + ": " + out["error"].asString());
    out["value"] = "error";
  }

  asyncWrite(out);
}



// called by Asio when the buffer has been written to the socket
void Session::processWrite(const boost::system::error_code& error)
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
  for (std::size_t j = 0; j < str.size(); j++)
    buffer_[j] = str[j];

  boost::asio::async_write(
      socket_, boost::asio::buffer(buffer_, str.size()),
      makeHandler(allocator_,
                  boost::bind(&Session::processWrite, shared_from_this(),
                              boost::asio::placeholders::error)));
}
