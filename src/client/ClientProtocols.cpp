
#include "ClientProtocols.hpp"
#include "../common/records/Registration.hpp"
#include "../common/Environment.hpp"
#include "../common/utils.hpp"
#include <botan/ber_dec.h>
#include <botan/sha2_32.h>
#include <iostream>


std::shared_ptr<ClientProtocols> ClientProtocols::singleton_ = 0;
std::shared_ptr<ClientProtocols> ClientProtocols::get()
{
   if (singleton_)
      return singleton_;

   singleton_ = std::make_shared<ClientProtocols>();
   return singleton_;
}



void ClientProtocols::proxyResolveListen()
{
   auto env  = Environment::get();
   auto inPipe = env->getReadPipe();
   auto outPipe = env->getWritePipe();

   std::cout << "client: " << env->getReadPipe() << std::endl;

   std::cout << "Listening on pipe \"" << inPipe << "\" ..." << std::endl;

   //while loop here to read in

   auto domainIn = "example.tor";
   std::cout << "Proxying \"" << domainIn << "\" to resolver..." << std::endl;

   auto onionOut = proxyDomainResolves(domainIn);

   std::cout << "Resolved \"" << domainIn << "\" to " << onionOut << std::endl;

   //end while
}



std::string ClientProtocols::proxyDomainResolves(const std::string& domain)
{
   //while loop here

   auto resolution = "2v7ibl5u4pbemwiz.onion";
   std::cout << "   \"" << domain << "\" -> " << resolution << std::endl;

   //end while

   return resolution;
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
