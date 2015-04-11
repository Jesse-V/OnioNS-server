
#include "SocksClient.hpp"
#include "SocksRequest.hpp"
#include "SocksReply.hpp"
#include <iostream>

using boost::asio::ip::tcp;


SocksClient::SocksClient(const std::string& socksIP,
   short socksPort, boost::asio::io_service& ios):
  ios_(ios), socksIP_(socksIP), socksPort_(socksPort), socket_(ios), resolver_(ios)
{
   tcp::resolver::query socks_query(socksIP, std::to_string(socksPort));
   tcp::resolver::iterator endpoint_iterator = resolver_.resolve(socks_query);
   boost::asio::connect(socket_, endpoint_iterator);
}



void SocksClient::connectTo(const std::string& host, short port)
{
   tcp::resolver::query query(tcp::v4(), host, "http");
   endpoint_ = *resolver_.resolve(query);
}



std::string SocksClient::sendReceive(const std::string& send)
{
   SocksRequest sreq(SocksRequest::connect, endpoint_, "OnioNS");
   boost::asio::write(socket_, sreq.buffers());

   SocksReply srep;
   boost::asio::read(socket_, srep.buffers());

   if (!srep.success())
   {
      std::cout << "Connection failed.\n";
      std::cout << "status = 0x" << std::hex << srep.status();
      return "";
   }

   std::string request =
      "GET / HTTP/1.0\r\n"
      "Host: www.jessevictors.com\r\n"
      "Accept: */*\r\n"
      "Connection: close\r\n\r\n";

   boost::asio::write(socket_, boost::asio::buffer(request));

   boost::array<char, 512> response;
   boost::system::error_code error;
   while (std::size_t s = socket_.read_some(boost::asio::buffer(response), error))
      std::cout.write(response.data(), s);

   if (error != boost::asio::error::eof)
      throw boost::system::system_error(error);

   return "hey";
}
