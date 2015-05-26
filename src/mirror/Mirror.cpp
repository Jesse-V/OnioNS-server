
#include "Mirror.hpp"
#include "tcp/Server.hpp"
#include <iostream>


void Mirror::startServer()
{
  Server s(10053);
  s.start();
}
