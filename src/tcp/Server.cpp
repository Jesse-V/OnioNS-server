
#include "Server.hpp"
#include "../Mirror.hpp"
#include <onions-common/Log.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;
using boost::asio::ip::address;


Server::Server(const std::string& bindIP, bool isQNode)
    : ios_(std::make_shared<boost::asio::io_service>()),
      acceptor_(
          *ios_,
          tcp::endpoint(address::from_string(bindIP), Const::SERVER_PORT)),
      isQNode_(isQNode)
{
  Log::get().notice("Initiating server...");

  auto socket = std::make_shared<boost::asio::ip::tcp::socket>(*ios_);
  acceptor_.async_accept(*socket,
                         boost::bind(&Server::handleAccept, this,
                                     boost::make_shared<Session>(socket, 1),
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
  static int sessionCounter = 2;
  int id = sessionCounter;
  sessionCounter++;

  if (error)
  {
    Log::get().warn(error.message());
    return;
  }

  Log::get().notice("Connection accepted (#" + std::to_string(id - 1) + ")");

  session->asyncRead();

  auto socket = std::make_shared<boost::asio::ip::tcp::socket>(*ios_);
  session.reset(new Session(socket, id));  // todo: use make_shared
  acceptor_.async_accept(*socket,
                         boost::bind(&Server::handleAccept, this, session,
                                     boost::asio::placeholders::error));
}
