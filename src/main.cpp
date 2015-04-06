

#include "main.hpp"
#include "common/Environment.hpp"
#include "client/ClientProtocols.hpp"
#include <botan/botan.h>
#include <iostream>

Botan::LibraryInitializer init("thread_safe");

int main(int argc, char** argv)
{
   //initialize environment
   auto env = Environment::get();
   env->setReadPipe("/tmp/tor-onions-in");
   env->setWritePipe("/tmp/tor-onions-out");

   //begin functionality
   auto clientProto = ClientProtocols::get();
   clientProto->proxyResolveListen();

   return EXIT_SUCCESS;
}




