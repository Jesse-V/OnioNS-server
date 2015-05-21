
#include "IPC.hpp"
#include <boost/bind.hpp>

using boost::asio::ip::tcp;


IPC::IPC(ushort port)
    : ios_(std::make_shared<boost::asio::io_service>()),
      acceptor_(*ios_, tcp::endpoint(tcp::v4(), port))
{
  std::cout << "Initiating Tor-OnioNS IPC socket..." << std::endl;
  boost::shared_ptr<IPCSession> session(new IPCSession(*ios_));
  acceptor_.async_accept(session->getSocket(),
                         boost::bind(&IPC::handleAccept, this, session,
                                     boost::asio::placeholders::error));
}



IPC::~IPC()
{
  std::cout << "Stopping IPC socket..." << std::endl;
  acceptor_.cancel();
}



void IPC::start()
{
  std::cout << "Starting IPC socket..." << std::endl;
  ios_->run();
}



void IPC::handleAccept(boost::shared_ptr<IPCSession> session,
                       const boost::system::error_code& error)
{
  std::cout << "IPC connection accepted." << std::endl;
  if (error)
  {
    std::cerr << error.message() << std::endl;
    return;
  }

  session->start();
  session.reset(new IPCSession(*ios_));
  acceptor_.async_accept(session->getSocket(),
                         boost::bind(&IPC::handleAccept, this, session,
                                     boost::asio::placeholders::error));
}
