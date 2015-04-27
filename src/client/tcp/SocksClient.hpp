
#ifndef SOCKS_CLIENT_HPP
#define SOCKS_CLIENT_HPP

#include <string>
#include <boost/asio.hpp>

class SocksClient
{
   public:
      SocksClient(const std::string&, short);
      void connectTo(const std::string&, short);
      std::string sendReceive(const std::string& send);

   private:
      bool checkSOCKS();

      std::shared_ptr<boost::asio::io_service> ios_;
      boost::asio::ip::tcp::endpoint endpoint_;
      boost::asio::ip::tcp::socket socket_;
      boost::asio::ip::tcp::resolver resolver_;
};

#endif
