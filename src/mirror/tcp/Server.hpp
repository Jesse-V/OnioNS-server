
#ifndef SERVER_HPP
#define SERVER_HPP

#include "Session.hpp"
#include <boost/asio.hpp>
#include <string>

class Server
{
 public:
  Server(ushort);
  ~Server();
  void start();
  void stop();

 private:
  void handleAccept(boost::shared_ptr<Session>,
                    const boost::system::error_code&);

  std::shared_ptr<boost::asio::io_service> ios_;
  boost::asio::ip::tcp::acceptor acceptor_;
};

#endif
