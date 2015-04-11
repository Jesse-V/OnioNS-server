
#include "Server.hpp"
#include <boost/bind.hpp>


Server::Server(boost::asio::io_service& ios, short port):
   ios_(ios),
   acceptor_(ios, tcp::endpoint(tcp::v4(), port))
{
   SessionPtr session(new Session(ios));
   acceptor_.async_accept(session->getSocket(),
      boost::bind(&Server::handleAccept, this, session,
      boost::asio::placeholders::error));
}



void Server::handleAccept(SessionPtr session, const boost::system::error_code& error)
{
   if (!error)
   {
      session->Start();
   }

   session.reset(new Session(ios_));
   acceptor_.async_accept(session->getSocket(),
      boost::bind(&Server::handleAccept, this, session,
      boost::asio::placeholders::error));
}
