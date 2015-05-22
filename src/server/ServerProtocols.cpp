
#include "ServerProtocols.hpp"
#include "tcp/Server.hpp"
#include <iostream>


void ServerProtocols::startServer()
{
  Server s(10053);
  s.start();
}
