
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



tcp::socket& Session::getSocket()
{
   return socket_;
}



void Session::Start()
{
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
   if (!error)
   {
      boost::asio::async_write(socket_,
         boost::asio::buffer(buffer_, n),
         makeHandler(allocator_,
            boost::bind(&Session::handleWrite,
            shared_from_this(),
            boost::asio::placeholders::error)));
   }
}



void Session::handleWrite(const boost::system::error_code& error)
{
   if (!error)
   {
      socket_.async_read_some(boost::asio::buffer(buffer_),
         makeHandler(allocator_,
         boost::bind(&Session::handleRead,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred)));
   }
}
