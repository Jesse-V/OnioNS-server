

#include "main.hpp"
#include "Flags.hpp"
#include "common/Environment.hpp"
#include "client/ClientProtocols.hpp"
#include "server/ServerProtocols.hpp"
#include <botan/botan.h>
#include <fstream>
#include <iostream>

Botan::LibraryInitializer init("thread_safe");

int main(int argc, char** argv)
{
   if (!Flags::get()->parse(argc, argv))
      return EXIT_SUCCESS;

   if (!Flags::get()->verbosityEnabled())
   { //https://bbs.archlinux.org/viewtopic.php?id=79378
      //https://groups.google.com/forum/#!topic/comp.lang.c++.moderated/ggl_2Ii3aVM
      //http://www.velocityreviews.com/forums/showpost.php?p=1501892&postcount=2
      std::ofstream nullOut("/dev/null");
      std::cout.rdbuf(nullOut.rdbuf());
   }

   //initialize environment
   auto env = Environment::get();
   env->setQueryPipe("/tmp/tor-onions-query");
   env->setResponsePipe("/tmp/tor-onions-response");

   if (Flags::get()->getMode() == Flags::OperationMode::CLIENT)
   {
      auto clientProto = ClientProtocols::get();
      clientProto->listenForDomains();
   }

   if (Flags::get()->getMode() == Flags::OperationMode::SERVER)
   {
      ServerProtocols::get().listenForDomains();
   }

   return EXIT_SUCCESS;
}
