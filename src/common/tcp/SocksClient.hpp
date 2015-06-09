
#ifndef SOCKS_CLIENT_HPP
#define SOCKS_CLIENT_HPP

#include <string>
#include <json/json.h>
#include <boost/asio.hpp>
#include <memory>

class SocksClient
{
 public:
  SocksClient(const std::string&, short);
  static std::shared_ptr<SocksClient> getCircuitTo(const std::string&);
  Json::Value sendReceive(const std::string&, const std::string&);

 private:
  void connectTo(const std::string&, short);
  bool checkConnection();

  std::shared_ptr<boost::asio::io_service> ios_;
  boost::asio::ip::tcp::endpoint endpoint_;
  boost::asio::ip::tcp::socket socket_;
  boost::asio::ip::tcp::resolver resolver_;
};

#endif
