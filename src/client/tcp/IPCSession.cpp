
#include "IPCSession.hpp"
#include "../ClientProtocols.hpp"
#include "../../common/tcp/MemAllocator.hpp"
#include "../../common/utils.hpp"
#include <boost/bind.hpp>
#include <algorithm>
#include <fstream>


template <typename Handler>
inline MemAllocator<Handler> makeHandler(HandleAlloc& a, Handler h)
{
  return MemAllocator<Handler>(a, h);
}



IPCSession::IPCSession(boost::asio::io_service& ios) : socket_(ios)
{
}



boost::asio::ip::tcp::socket& IPCSession::getSocket()
{
  return socket_;
}



void IPCSession::start()
{
  asyncReadBuffer();
}



void IPCSession::processRead(const boost::system::error_code& error, size_t n)
{
  if (error || n <= 0)
  {
    std::cout << std::endl;
    std::cerr << error.message() << std::endl;
    return;
  }

  std::string domainIn(buffer_.begin(), buffer_.begin() + n);
  std::cout << "Read \"" << domainIn << "\" from Tor Browser." << std::endl;
  std::string onionOut = ClientProtocols::get().resolve(domainIn);
  std::cout << "Writing \"" << onionOut << "\" to Tor Browser... ";

  for (std::size_t j = 0; j < onionOut.size(); j++)
    buffer_[j] = onionOut[j];
  asyncWriteBuffer(onionOut.size());
}



// called by Asio when the buffer has been written to the socket
void IPCSession::processWrite(const boost::system::error_code& error)
{
  if (error)
  {
    std::cerr << error.message() << std::endl;
    return;
  }

  std::cout << " write complete. \n";
  asyncReadBuffer();
}



// ***************************** PRIVATE METHODS *****************************



void IPCSession::asyncReadBuffer()
{
  socket_.async_read_some(
      boost::asio::buffer(buffer_),
      makeHandler(allocator_,
                  boost::bind(&IPCSession::processRead, shared_from_this(),
                              boost::asio::placeholders::error,
                              boost::asio::placeholders::bytes_transferred)));
}



void IPCSession::asyncWriteBuffer(std::size_t len)
{
  boost::asio::async_write(
      socket_, boost::asio::buffer(buffer_, len),
      makeHandler(allocator_,
                  boost::bind(&IPCSession::processWrite, shared_from_this(),
                              boost::asio::placeholders::error)));
}
