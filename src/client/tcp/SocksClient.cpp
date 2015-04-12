
#include "SocksClient.hpp"
#include "SocksRequest.hpp"
#include "SocksReply.hpp"
#include <iostream>

using boost::asio::ip::tcp;


SocksClient::SocksClient(const std::string& socksIP, short socksPort):
   ios_(std::make_shared<boost::asio::io_service>()),
   socksIP_(socksIP), socksPort_(socksPort), socket_(*ios_), resolver_(*ios_)
{
   std::cout << "Creating SOCKS connection..." << std::endl;
   tcp::resolver::query socks_query(socksIP, std::to_string(socksPort));
   tcp::resolver::iterator endpoint_iterator = resolver_.resolve(socks_query);
   boost::asio::connect(socket_, endpoint_iterator);
}



void SocksClient::connectTo(const std::string& host, short port)
{
   std::cout << "Connecting to remove host..." << std::endl;
   tcp::resolver::query query(tcp::v4(), host, std::to_string(port));
   endpoint_ = *resolver_.resolve(query);
}



std::string SocksClient::sendReceive(const std::string& sendStr)
{
   if (!checkSOCKS())
      throw std::runtime_error("Remote host refused connection.");

   std::cout << "Writing \"" << sendStr << "\" ..." << std::endl;
   boost::asio::write(socket_, boost::asio::buffer(sendStr));

   std::cout << "Reading response line..." << std::endl;

   boost::asio::streambuf response;
   boost::asio::read_until(socket_, response, "\n");

   std::string s;
   std::istream is(&response);
   is >> s;

   std::cout << "Remote host returned \"" << s << "\"" << std::endl;
   return s;
}


bool SocksClient::checkSOCKS()
{
   std::cout << "Checking SOCKS..." << std::endl;

   SocksRequest sreq(SocksRequest::connect, endpoint_, "OnioNS");
   boost::asio::write(socket_, sreq.buffers());

   SocksReply srep;
   boost::asio::read(socket_, srep.buffers());

   if (!srep.success())
   {
      std::cout << "Connection failed.\n";
      std::cout << "status = 0x" << std::hex << srep.status();
      return false;
   }

   return true;
}
