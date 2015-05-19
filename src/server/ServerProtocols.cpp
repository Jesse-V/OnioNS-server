
#include "ServerProtocols.hpp"
#include "tcp/Server.hpp"
#include <iostream>


void ServerProtocols::startServer()
{
  Server s(15678);
  s.start();
}
