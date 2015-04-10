
#include "ServerProtocols.hpp"
#include <iostream>
#include <memory>
#include <iostream>
#include <chrono>
#include <thread>


std::shared_ptr<ServerProtocols> ServerProtocols::singleton_ = 0;
std::shared_ptr<ServerProtocols> ServerProtocols::get()
{
   if (singleton_)
      return singleton_;

   singleton_ = std::make_shared<ServerProtocols>();
   return singleton_;
}



ServerProtocols::ServerProtocols():
   service_(std::make_shared<boost::asio::io_service>())
{}



void ServerProtocols::listenForDomains()
{
   start(5678);
   service_->run();
   std::cout << "done!" << std::endl;
}



void ServerProtocols::start(int port)
{
   acceptor_ = std::shared_ptr<tcp::acceptor>( new tcp::acceptor(*service_,
      tcp::endpoint(tcp::v4(), port)));

   std::cout << "Starting..." << std::endl;

   accept();
}



void ServerProtocols::stop()
{
   std::cout << "Stopping" << std::endl;
   acceptor_->cancel();
}



void ServerProtocols::accept()
{
   std::cout << "accept" << std::endl;
   std::shared_ptr<tcp::socket> pSocket(new tcp::socket(*service_));

   acceptor_->async_accept(*pSocket, bind(&ServerProtocols::handleAccept, this, pSocket));
}



void ServerProtocols::handleAccept(std::shared_ptr<tcp::socket> pSocket)
{
   std::cout << "handleAccept" << std::endl;
   serve(pSocket);
   accept();
}



void ServerProtocols::serve(std::shared_ptr<tcp::socket> pSocket)
{
   std::cout << "serving..." << std::endl;
}
