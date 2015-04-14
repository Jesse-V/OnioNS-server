
#include "ServerProtocols.hpp"
#include "tcp/Server.hpp"
#include <iostream>


ServerProtocols::ServerProtocols()
{}



void ServerProtocols::startServer()
{
   Server s(15678);
   s.start();
}

