
#ifndef SERVER_HPP
#define SERVER_HPP

#include "Session.hpp"
#include <boost/asio.hpp>
#include <string>

typedef boost::shared_ptr<Session> SessionPtr;

class Server
{
   public:
      Server(short);
      ~Server();
      void start();
      void stop();

   private:
      void handleAccept(SessionPtr, const boost::system::error_code&);

      std::shared_ptr<boost::asio::io_service> ios_;
      boost::asio::ip::tcp::acceptor acceptor_;
};

#endif
