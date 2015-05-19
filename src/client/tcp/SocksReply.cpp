
#include "SocksReply.hpp"


SocksReply::SocksReply() : null_byte_(0), status_()
{
}



boost::array<boost::asio::mutable_buffer, 5> SocksReply::buffers()
{
  boost::array<boost::asio::mutable_buffer, 5> bufs = {
      {boost::asio::buffer(&null_byte_, 1),
       boost::asio::buffer(&status_, 1),
       boost::asio::buffer(&port_high_byte_, 1),
       boost::asio::buffer(&port_low_byte_, 1),
       boost::asio::buffer(address_)}};

  return bufs;
}



bool SocksReply::success() const
{
  return null_byte_ == 0 && status_ == request_granted;
}



unsigned char SocksReply::status() const
{
  return status_;
}



boost::asio::ip::tcp::endpoint SocksReply::endpoint() const
{
  unsigned short port = port_high_byte_;
  port = (port << 8) & 0xff00;
  port = port | port_low_byte_;

  boost::asio::ip::address_v4 address(address_);
  return boost::asio::ip::tcp::endpoint(address, port);
}
