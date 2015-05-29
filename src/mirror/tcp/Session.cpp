
#include "Session.hpp"
#include "../../common/tcp/MemAllocator.hpp"
#include "../../common/utils.hpp"
#include "../containers/Cache.hpp"
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

  std::string inputStr(buffer_.begin(), buffer_.begin() + n);
  Json::Value inputVal, outputVal;

  Json::Reader reader;
  if (!reader.parse(inputStr, inputVal))
  {
    outputVal["error"] = "Failed to parse message!";
    asyncWrite(outputVal);
    return;
  }

  if (!inputVal.isMember("request"))
  {
    outputVal["error"] = "Message is missing the request field!";
    asyncWrite(outputVal);
    return;
  }

  // todo: can confirm that it can be casted to string
  std::string request(inputVal["request"].asString());

  if (request == "ping")
  {
    outputVal["response"] = "pong";
  }
  else if (Utils::strEndsWith(request, ".tor"))
  {  // resolve .tor -> .onion

    auto record = Cache::get().get(request);
    if (record)
    {
      outputVal["response"] = record->asJSON();
      std::cout << "Found Record for \"" << request << "\"" << std::endl;
    }
    else
    {
      outputVal["response"] = "404";
      std::cout << "404ed request for \"" << request << "\"" << std::endl;
    }
  }
  else
    outputVal["error"] = "Unknown request.";

  asyncWrite(outputVal);
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



// ***************************** PRIVATE METHODS *****************************



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
