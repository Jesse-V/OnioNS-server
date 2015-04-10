

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
   env->setQueryPipe("/tmp/tor-onions-query");
   env->setResponsePipe("/tmp/tor-onions-response");

   //begin functionality
   auto clientProto = ClientProtocols::get();
   clientProto->listenForDomains();

   return EXIT_SUCCESS;
}




