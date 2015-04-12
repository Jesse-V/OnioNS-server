
#include "ServerProtocols.hpp"
#include "tcp/Server.hpp"
#include <iostream>


ServerProtocols::ServerProtocols()
{}



void ServerProtocols::startServer()
{
   Server s(5678);
   s.start();
}

