
#ifndef SOCKS_CLIENT_HPP
#define SOCKS_CLIENT_HPP

#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class SocksClient
{
   public:
      SocksClient(const std::string&, short, boost::asio::io_service&);
      void connectTo(const std::string&, short);
      std::string sendReceive(const std::string& send);

   private:
      boost::asio::io_service& ios_;
      std::string socksIP_;
      short socksPort_;
      tcp::endpoint endpoint_;
      tcp::socket socket_;
      tcp::resolver resolver_;
};

#endif
