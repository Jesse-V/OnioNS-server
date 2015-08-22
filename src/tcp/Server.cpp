
#include "Server.hpp"
#include "../Mirror.hpp"
#include <onions-common/Log.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;
using boost::asio::ip::address;


Server::Server(bool isQNode)
    : ios_(std::make_shared<boost::asio::io_service>()),
      acceptor_(*ios_,
                tcp::endpoint(address::from_string("127.53.53.53"), 10053)),
      isQNode_(isQNode)
{
  Log::get().notice("Initiating server...");
  boost::shared_ptr<Session> session(new Session(*ios_, 0));
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



void Server::handleAccept(boost::shared_ptr<Session> session,
                          const boost::system::error_code& error)
{
  static int sessionCounter = 1;
  int id = sessionCounter;
  sessionCounter++;

  if (error)
  {
    Log::get().warn(error.message());
    return;
  }

  Log::get().notice("Connection accepted (#" + std::to_string(id - 1) + ")");

  session->asyncRead();

  session.reset(new Session(*ios_, id));
  acceptor_.async_accept(session->getSocket(),
                         boost::bind(&Server::handleAccept, this, session,
                                     boost::asio::placeholders::error));
}
