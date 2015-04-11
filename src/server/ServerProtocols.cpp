
#include "ServerProtocols.hpp"
#include <iostream>
#include <memory>
#include <iostream>
#include <chrono>
#include <thread>


ServerProtocols::ServerProtocols():
   service_(std::make_shared<boost::asio::io_service>())
{}



void ServerProtocols::listenForDomains()
{
   start(5678);
   service_->run();


   //boost::asio::ip::tcp::iostream stream("www.example.org", "http");
   //stream << "GET / HTTP/1.0\r\nHost: www.boost.org\r\n\r\n" << std::flush;
   //std::string response;
   //std::getline( stream, response );
}



void ServerProtocols::start(int port)
{
   acceptor_ = std::make_shared<tcp::acceptor>(*service_,
      tcp::endpoint(tcp::v4(), port));

   std::cout << "Starting server..." << std::endl;
   accept();
}



void ServerProtocols::stop()
{
   std::cout << "Stopping" << std::endl;
   acceptor_->cancel();
}



void ServerProtocols::accept()
{
   auto socket = std::make_shared<tcp::socket>(*service_);
   auto b = bind(&ServerProtocols::handleAccept, this, socket);
   acceptor_->async_accept(*socket, b);
}



void ServerProtocols::handleAccept(std::shared_ptr<tcp::socket> socket)
{
   std::cout << "handleAccept" << std::endl;
   std::cout.flush();
   serve(socket);
   accept(); //todo: fix
}



void ServerProtocols::serve(std::shared_ptr<tcp::socket> socket)
{
   std::cout << "serving..." << std::endl;

   //auto b = bind(&ServerProtocols::handleAccept, this, socket);

   boost::asio::streambuf response(1024);
   boost::asio::sync_read_until(*socket, response, '\n');

   std::istream is(&response);
   std::string s;
   is >> s;
   std::cout << "**" << s << "**" << std::endl;

   boost::asio::streambuf b;
   std::ostream os(&b);
   os << "Hello, World!\n";

   size_t n = socket->send(b.data());
   std::cout << "Sent " << n << " bytes!" << std::endl;
}


void ServerProtocols::resolve()
{
   std::cout << "Resolve!" << std::endl;
}
