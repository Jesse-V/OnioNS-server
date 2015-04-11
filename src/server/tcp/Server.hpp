
#ifndef SERVER_HPP
#define SERVER_HPP

#include "Session.hpp"
#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

typedef boost::shared_ptr<Session> SessionPtr;

class Server
{
   public:
      Server(boost::asio::io_service&, short);
      void handleAccept(SessionPtr, const boost::system::error_code&);

   private:
      boost::asio::io_service& ios_;
      tcp::acceptor acceptor_;
};

#endif
