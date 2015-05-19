
#ifndef SOCKS_REQUEST_HPP
#define SOCKS_REQUEST_HPP

#include <boost/array.hpp>
#include <boost/asio.hpp>

// adapted from Boost's 1.53 socks4 sync_client.cpp example
class SocksRequest
{
 public:
  enum command_type
  {
    connect = 0x01,
    bind = 0x02
  };

  SocksRequest(command_type,
               const boost::asio::ip::tcp::endpoint&,
               const std::string&);

  boost::array<boost::asio::const_buffer, 7> buffers() const;

 private:
  unsigned char version_;
  unsigned char command_;
  unsigned char port_high_byte_;
  unsigned char port_low_byte_;
  boost::asio::ip::address_v4::bytes_type address_;
  std::string user_id_;
  unsigned char null_byte_;
};

#endif
