
#include "Session.hpp"
#include "MemAllocator.hpp"
#include <boost/bind.hpp>


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
   std::cout << "Starting session... Reading..." << std::endl;
   socket_.async_read_some(boost::asio::buffer(buffer_),
      makeHandler(allocator_,
         boost::bind(&Session::handleRead,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred)));
}



void Session::handleRead(const boost::system::error_code& error,
   size_t n)
{
   if (error)
   {
      std::cerr << error.message() << std::endl;
      return;
   }

   //auto resolution = "2v7ibl5u4pbemwiz\0";
   /* resolve .tor -> .onion */
      //char* resolution = "2v7ibl5u4pbemwiz\0";
      //"blkbook3fxhcsn3u\0";
      //"uhwikih256ynt57t\0";
      //"2v7ibl5u4pbemwiz\0";

   std::cout << "Writing..." << std::endl;
   boost::asio::async_write(socket_,
      boost::asio::buffer(buffer_, n),
      makeHandler(allocator_,
         boost::bind(&Session::handleWrite,
         shared_from_this(),
         boost::asio::placeholders::error)));
}



void Session::handleWrite(const boost::system::error_code& error)
{
   if (error)
   {
      std::cerr << error.message() << std::endl;
      return;
   }

   std::cout << "Reading..." << std::endl;
   socket_.async_read_some(boost::asio::buffer(buffer_),
      makeHandler(allocator_,
      boost::bind(&Session::handleRead,
         shared_from_this(),
         boost::asio::placeholders::error,
         boost::asio::placeholders::bytes_transferred)));
}
