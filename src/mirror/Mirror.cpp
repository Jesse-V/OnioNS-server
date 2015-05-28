
#include "Mirror.hpp"
#include "tcp/Server.hpp"
#include <iostream>


void Mirror::startServer()
{
  // todo: load Records from the cache file, add to Cache

  Server s(Environment::SERVER_PORT);
  s.start();
}
