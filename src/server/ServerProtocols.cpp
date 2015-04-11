
#include "ServerProtocols.hpp"
#include "tcp/Server.hpp"
#include <boost/asio.hpp>
#include <iostream>


ServerProtocols::ServerProtocols()
{}



void ServerProtocols::listenForDomains()
{
   boost::asio::io_service ios;
   Server s(ios, 5678);
   ios.run();
}



/*
void ServerProtocols::stop()
{
   std::cout << "Stopping" << std::endl;
   acceptor_->cancel();
}
*/
