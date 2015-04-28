
#include "Session.hpp"
#include "MemAllocator.hpp"
#include "../../common/utils.hpp"
#include <boost/bind.hpp>
#include <algorithm>


template <typename Handler>
inline MemAllocator<Handler> makeHandler(
   HandleAlloc& a, Handler h)
{
   return MemAllocator<Handler>(a, h);
}



Session::Session(boost::asio::io_service& ios):
   socket_(ios)
{}



boost::asio::ip::tcp::socket& Session::getSocket()
{
   return socket_;
}



void Session::start()
{
   std::cout << "Starting session..." << std::endl;
   asyncReadBuffer();
}



//called by Asio whenever the socket has been read into the buffer
void Session::processRead(const boost::system::error_code& error, size_t n)
{
   if (error || n <= 0)
   {
      std::cout << std::endl;
      std::cerr << error.message() << std::endl;
      return;
   }

   std::string domainIn(buffer_.begin(), buffer_.begin() + n);
   std::string response("404");

   std::size_t end = std::min(domainIn.find('\r'), domainIn.find('\n'));
   if (end == std::string::npos)
   {
      std::cout << "    *" << domainIn << "*" << std::endl;
      std::cerr << "No newline found!" << std::endl;
      response = "<MALFORMED>";
   }
   else
   {
      domainIn.resize(end);
      std::cout << "received \"" << domainIn << "\"" << std::endl;

      if (Utils::strEndsWith(domainIn, ".tor"))
      { //resolve .tor -> .onion

         response = "onions55e7yam27n.onion";
         //response = "2v7ibl5u4pbemwiz.onion";
         //response = "blkbook3fxhcsn3u.onion";
         //response = "uhwikih256ynt57t.onion";
      }

      std::cout << "Server writes \"" << response << "\"" << std::endl;
   }

   for (std::size_t j = 0; j < response.size(); j++)
      buffer_[j] = response[j];
   buffer_[response.size() + 0] = '\r';
   buffer_[response.size() + 1] = '\n';
   asyncWriteBuffer(response.size() + 2);
}



//called by Asio when the buffer has been written to the socket
void Session::processWrite(const boost::system::error_code& error)
{
   if (error)
   {
      std::cerr << error.message() << std::endl;
      return;
   }

   std::cout << "done." << std::endl;
   asyncReadBuffer();
}



//************************** PRIVATE METHODS *************************



void Session::asyncReadBuffer()
{
   std::cout << "Reading... ";
   socket_.async_read_some(boost::asio::buffer(buffer_),
      makeHandler(allocator_, boost::bind(&Session::processRead,
         shared_from_this(), boost::asio::placeholders::error,
         boost::asio::placeholders::bytes_transferred)));
}



void Session::asyncWriteBuffer(std::size_t len)
{
   std::cout << "Writing... ";
   boost::asio::async_write(socket_,
      boost::asio::buffer(buffer_, len),
      makeHandler(allocator_, boost::bind(&Session::processWrite,
         shared_from_this(), boost::asio::placeholders::error)));
}
