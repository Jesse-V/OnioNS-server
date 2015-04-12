
#include "Server.hpp"
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

Server::Server(short port):
   ios_(std::make_shared<boost::asio::io_service>()),
   acceptor_(*ios_, tcp::endpoint(tcp::v4(), port))
{
   std::cout << "Initiating server..." << std::endl;
   SessionPtr session(new Session(*ios_));
   acceptor_.async_accept(session->getSocket(),
      boost::bind(&Server::handleAccept, this, session,
      boost::asio::placeholders::error));
}



Server::~Server()
{
   stop();
}



void Server::start()
{
   std::cout << "Starting server..." << std::endl;
   ios_->run();
}



void Server::stop()
{
   std::cout << "Stopping server..." << std::endl;
   acceptor_.cancel();
}



void Server::handleAccept(SessionPtr session, const boost::system::error_code& error)
{
   std::cout << "Connection accepted." << std::endl;
   if (error)
   {
      std::cerr << error.message() << std::endl;
      return;
   }

   session->start();
   session.reset(new Session(*ios_));
   acceptor_.async_accept(session->getSocket(),
      boost::bind(&Server::handleAccept, this, session,
      boost::asio::placeholders::error));
}
