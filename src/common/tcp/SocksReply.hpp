
#ifndef SOCKS_REPLY_HPP
#define SOCKS_REPLY_HPP

#include <boost/array.hpp>
#include <boost/asio.hpp>

// adapted from Boost's 1.53 socks4 sync_client.cpp example
class SocksReply
{
 public:
  enum status_type
  {
    request_granted = 0x5a,
    request_failed = 0x5b,
    request_failed_no_identd = 0x5c,
    request_failed_bad_user_id = 0x5d
  };

  SocksReply();
  boost::array<boost::asio::mutable_buffer, 5> buffers();
  bool success() const;
  unsigned char status() const;
  boost::asio::ip::tcp::endpoint endpoint() const;

 private:
  unsigned char null_byte_;
  unsigned char status_;
  unsigned char port_high_byte_;
  unsigned char port_low_byte_;
  boost::asio::ip::address_v4::bytes_type address_;
};

#endif
