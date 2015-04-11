
#include "ServerProtocols.hpp"
#include "tcp/Server.hpp"
#include <boost/asio.hpp>
#include <iostream>


ServerProtocols::ServerProtocols()
{}



void ServerProtocols::listenForDomains()
{
   auto service = std::make_shared<boost::asio::io_service>();
   Server s(*service, 5678);
   service->run();
}



/*
void ServerProtocols::stop()
{
   std::cout << "Stopping" << std::endl;
   acceptor_->cancel();
}
*/
