
#include "SocksRequest.hpp"


const unsigned char version = 0x04;


SocksRequest::SocksRequest(command_type cmd,
                           const boost::asio::ip::tcp::endpoint& endpoint,
                           const std::string& user_id)
    : version_(version), command_(cmd), user_id_(user_id), null_byte_(0)
{
  // Only IPv4 is supported by the SOCKS 4 protocol.
  if (endpoint.protocol() != boost::asio::ip::tcp::v4())
  {
    throw boost::system::system_error(
        boost::asio::error::address_family_not_supported);
  }

  // Convert port number to network byte order.
  unsigned short port = endpoint.port();
  port_high_byte_ = (port >> 8) & 0xff;
  port_low_byte_ = port & 0xff;

  // Save IP address in network byte order.
  address_ = endpoint.address().to_v4().to_bytes();
}



boost::array<boost::asio::const_buffer, 7> SocksRequest::buffers() const
{
  boost::array<boost::asio::const_buffer, 7> bufs = {
      {boost::asio::buffer(&version_, 1),
       boost::asio::buffer(&command_, 1),
       boost::asio::buffer(&port_high_byte_, 1),
       boost::asio::buffer(&port_low_byte_, 1),
       boost::asio::buffer(address_),
       boost::asio::buffer(user_id_),
       boost::asio::buffer(&null_byte_, 1)}};
  return bufs;
}
