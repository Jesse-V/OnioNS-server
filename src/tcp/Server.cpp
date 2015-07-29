
#include "Server.hpp"
#include "../Mirror.hpp"
#include <onions-common/Log.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;


Server::Server(const std::string& host, ushort port, bool isAuthority)
    : ios_(std::make_shared<boost::asio::io_service>()),
      acceptor_(
          *ios_,
          tcp::endpoint(boost::asio::ip::address::from_string(host.c_str()),
                        port)),
      isAuthorative_(isAuthority)
{
  Log::get().notice("Initiating server...");
  std::shared_ptr<Session> session(new Session(*ios_));
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
  Log::get().notice("Starting server...");
  ios_->run();
}



void Server::stop()
{
  Log::get().notice("Stopping server...");
  acceptor_.cancel();
}



void Server::handleAccept(std::shared_ptr<Session> session,
                          const boost::system::error_code& error)
{
  Log::get().notice("Connection accepted.");
  if (error)
  {
    Log::get().warn(error.message());
    return;
  }

  Mirror::addConnection(session);
  session->asyncRead();

  session.reset(new Session(*ios_));
  acceptor_.async_accept(session->getSocket(),
                         boost::bind(&Server::handleAccept, this, session,
                                     boost::asio::placeholders::error));
}
