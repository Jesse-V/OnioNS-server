
#include "ClientProtocols.hpp"
#include "../common/records/Registration.hpp"
#include "../common/Environment.hpp"
#include "../common/utils.hpp"
#include <botan/sha2_32.h>
#include <thread>
#include <sys/stat.h>
#include <fstream>
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
   createNamedPipes();
   pipeListen();
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



void ClientProtocols::createNamedPipes()
{
   auto env  = Environment::get();

   std::cout << "Initializing IPC... ";
   mkfifo(env->getReadPipe().c_str(),  0777);
   mkfifo(env->getWritePipe().c_str(), 0777);
   std::cout << "done." << std::endl;
}



void ClientProtocols::pipeListen()
{
   auto env  = Environment::get();
   const auto IN_PATH  = env->getReadPipe();
   const auto OUT_PATH = env->getWritePipe();

   std::cout << "Listening on pipe \"" << IN_PATH  << "\" ..." << std::endl;
   std::cout << "Resolving to pipe \"" << OUT_PATH << "\" ..." << std::endl;

   const auto POLL_DELAY = std::chrono::milliseconds(50);
   const int MAX_LEN = 256;

   //open pipes on this end
   std::ofstream outPipe(OUT_PATH);
   FILE* inPipe = fopen(IN_PATH.c_str(), "r"); //open named pipe

   //prepare reading buffer
   char* buffer = new char[MAX_LEN];
   memset(buffer, 0, MAX_LEN);

   while (true)
   {
      //read in a single line
      int index = 0;
      for (char c = getc(inPipe); c != '\n' && c != EOF && index < MAX_LEN;
         c = getc(inPipe))
      {
         buffer[index] = c;
         index++;
      }

      //process if something was read in
      if (index > 0)
      {
         //convert read to string
         std::string domainIn(buffer, index);

         //resolve
         std::cout << "Proxying \"" << domainIn << "\" to resolver..." << std::endl;
         auto onionOut = resolveByProxy(domainIn);
         std::cout << "Resolved \"" << domainIn << "\" to " << onionOut << std::endl;

         //return to Tor Browser
         outPipe << onionOut << std::endl;
         outPipe.flush();

         //reset read buffer
         memset(buffer, 0, MAX_LEN);
      }

      //delay before polling pipe again
      std::this_thread::sleep_for(POLL_DELAY);
   }

   //tear down file descriptors
   fclose(inPipe);
   outPipe.close();
}



std::string ClientProtocols::resolveByProxy(const std::string& domain)
{
   //while loop here

   auto resolution = "2v7ibl5u4pbemwiz.onion";
   std::cout << "   \"" << domain << "\" -> " << resolution << std::endl;

   //end while

   return resolution;
}
