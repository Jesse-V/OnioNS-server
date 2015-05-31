
#include "Session.hpp"
#include "../../common/Common.hpp"
#include "../../common/tcp/MemAllocator.hpp"
#include "../../common/utils.hpp"
#include "../containers/Cache.hpp"
#include <botan/sha2_64.h>
#include <botan/base64.h>
#include <boost/bind.hpp>
#include <algorithm>
#include <fstream>


template <typename Handler>
inline MemAllocator<Handler> makeHandler(HandleAlloc& a, Handler h)
{
  return MemAllocator<Handler>(a, h);
}



Session::Session(boost::asio::io_service& ios) : socket_(ios)
{
}



boost::asio::ip::tcp::socket& Session::getSocket()
{
  return socket_;
}



void Session::start()
{
  std::cout << "Starting session..." << std::endl;
  asyncRead();
}



// called by Asio whenever the socket has been read into the buffer
void Session::processRead(const boost::system::error_code& error, size_t n)
{
  if (error || n <= 0)
  {
    std::cout << std::endl;
    std::cerr << error.message() << std::endl;
    return;
  }

  Json::Value in, out;
  Json::Reader reader;
  std::string inputStr(buffer_.begin(), buffer_.begin() + n);

  if (!reader.parse(inputStr, in))
    out["error"] = "Failed to parse message!";
  else if (!in.isMember("command"))
    out["error"] = "Message is missing the \"command\" field!";
  else
  {
    std::string command(in["command"].asString());

    if (command == "ping")
      handlePing(in, out);
    else if (command == "proveKnowledge")
      handleProveKnowledge(in, out);
    else if (command == "upload")
      handleUpload(in, out);
    else if (command == "domainQuery")
      handleDomainQuery(in, out);
    else
      out["error"] = "Unknown command \"" + command + "\"\n";
  }

  if (!out.isMember("error"))
    out["response"] = "success";
  asyncWrite(out);
}



// ***************************** PRIVATE METHODS *****************************



void Session::handlePing(Json::Value& in, Json::Value& out)
{
  out["response"] = "pong";
}



void Session::handleProveKnowledge(Json::Value& in, Json::Value& out)
{
  auto r = Cache::get().get(in["domain"].asString());
  if (r)
  {
    Botan::SHA_384 sha;
    out["response"] =
        Botan::base64_encode(sha.process(r->asJSON()), Environment::SHA384_LEN);
  }
  else
    out["response"] = "404";
}



void Session::handleUpload(Json::Value& in, Json::Value& out)
{
  if (in.isMember("record"))
  {
    if (!Cache::get().add(Common::get().parseRecord(in["record"])))
      out["error"] = "Name already taken.";
  }

  else
    out["error"] = "Missing Record.";
}



void Session::handleDomainQuery(Json::Value& in, Json::Value& out)
{
  std::string domain = in["domain"].asString();
  if (Utils::strEndsWith(domain, ".tor"))
  {  // resolve .tor -> .onion

    auto record = Cache::get().get(domain);
    if (record)
    {
      out["response"] = record->asJSON();
      std::cout << "Found Record for \"" << domain << "\"" << std::endl;
    }
    else
    {
      out["response"] = "404";
      std::cout << "404ed request for \"" << domain << "\"" << std::endl;
    }
  }
  else
    out["error"] = "Invalid request.";
}



// called by Asio when the buffer has been written to the socket
void Session::processWrite(const boost::system::error_code& error)
{
  if (error)
  {
    std::cerr << error.message() << std::endl;
    return;
  }

  // std::cout << "done." << std::endl;
  asyncRead();
}



void Session::asyncRead()
{
  // std::cout << "Reading... ";
  socket_.async_read_some(
      boost::asio::buffer(buffer_),
      makeHandler(allocator_,
                  boost::bind(&Session::processRead, shared_from_this(),
                              boost::asio::placeholders::error,
                              boost::asio::placeholders::bytes_transferred)));
}



void Session::asyncWrite(const Json::Value& val)
{
  Json::FastWriter writer;
  asyncWrite(writer.write(val));
}



void Session::asyncWrite(const std::string& str)
{
  for (std::size_t j = 0; j < str.size(); j++)
    buffer_[j] = str[j];

  // std::cout << "Writing... ";
  boost::asio::async_write(
      socket_, boost::asio::buffer(buffer_, str.size()),
      makeHandler(allocator_,
                  boost::bind(&Session::processWrite, shared_from_this(),
                              boost::asio::placeholders::error)));
}
