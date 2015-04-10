
#include "ClientProtocols.hpp"
#include "../common/records/Registration.hpp"
#include "../common/Environment.hpp"
#include "../common/utils.hpp"
#include <botan/sha2_32.h>
#include <sys/stat.h>
#include <thread>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>


std::shared_ptr<ClientProtocols> ClientProtocols::singleton_ = 0;
std::shared_ptr<ClientProtocols> ClientProtocols::get()
{
   if (singleton_)
      return singleton_;

   singleton_ = std::make_shared<ClientProtocols>();
   return singleton_;
}



void ClientProtocols::listenForDomains()
{
   //const auto PIPE_CHECK = std::chrono::milliseconds(500);
   auto env = Environment::get();
   const auto QUERY_PATH    = env->getQueryPipe();
   const auto RESPONSE_PATH = env->getResponsePipe();
   const auto POLL_DELAY = std::chrono::milliseconds(500);
   const int MAX_LEN = 256;

   //create named pipes that we will use for Tor-OnioNS IPC
   std::cout << "Initializing IPC... ";
   mkfifo(QUERY_PATH.c_str(),    0777);
   mkfifo(RESPONSE_PATH.c_str(), 0777);
   std::cout << "done." << std::endl;

   //named pipes are best dealt with C-style
   //each side has to open for reading before the other can open for writing
   std::cout << "Waiting for Tor connection... 1 ";
   std::cout.flush();
   int responsePipe = open(RESPONSE_PATH.c_str(), O_WRONLY);
   std::cout << "2 ";
   std::cout.flush();
   int queryPipe    = open(QUERY_PATH.c_str(),    O_RDONLY);
   std::cout << "done. " << std::endl;

   std::cout << "Listening on pipe \"" << QUERY_PATH  << "\" ..." << std::endl;
   std::cout << "Resolving to pipe \"" << RESPONSE_PATH << "\" ..." << std::endl;

   //prepare reading buffer
   char* buffer = new char[MAX_LEN + 1];
   memset(buffer, 0, MAX_LEN);

   while (true)
   {
      int readLength = read(queryPipe, (void*)buffer, MAX_LEN);
      if (readLength < 0)
      {
         std::cerr << "Read error from IPC named pipe!" << std::endl;
      }
      else if (readLength > 0)
      {
         //terminate buffer
         buffer[readLength] = '\0';

         //convert read to string
         std::string domainIn(buffer, readLength - 1);

         //resolve
         std::cout << "Proxying \"" << domainIn << "\" to resolver..." << std::endl;
         auto onionOut = resolveByProxy(domainIn);
         std::cout << "Resolved \"" << domainIn << "\" to " << onionOut << std::endl;

         //flush result to Tor Browser
         //outPipe << onionOut << std::endl;
         //outPipe.flush();
      }

      //delay before polling pipe again
      std::this_thread::sleep_for(POLL_DELAY);
   }

   //tear down file descriptors
   close(queryPipe);
   close(responsePipe);
}



std::shared_ptr<Record> generateRecord()
{
   try
   {
      Botan::AutoSeeded_RNG rng;
      Botan::RSA_PrivateKey* rsaKey = Utils::loadKey("assets/example.key", rng);
      if (rsaKey != NULL)
         std::cout << "RSA private key loaded successfully!" << std::endl;

      Botan::SHA_256 sha;
      auto hash = sha.process("hello world");

      uint8_t cHash[32];
      memcpy(cHash, hash, 32);

      auto r = std::make_shared<Registration>(rsaKey, cHash,
         "example.tor", "AD97364FC20BEC80");

      std::cout << std::endl;
      std::cout << "Initial JSON: " << r->asJSON() << std::endl;

      r->makeValid(4);

      std::cout << std::endl;
      std::cout << "Result:" << std::endl;
      std::cout << r << std::endl;

      std::cout << std::endl;
      std::cout << "Final JSON: " << r->asJSON() << std::endl;

      return r;
   }
   catch (std::exception& e)
   {
      std::cerr << e.what() << "\n";
   }

   return NULL;
}



// *********************** PRIVATE METHODS: *********************



std::string ClientProtocols::resolveByProxy(const std::string& domain)
{
   //while loop here

   auto resolution = "2v7ibl5u4pbemwiz.onion";
   std::cout << "   \"" << domain << "\" -> " << resolution << std::endl;

   //end while

   return resolution;
}
