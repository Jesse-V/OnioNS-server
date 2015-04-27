
#include "HSProtocols.hpp"
#include "../common/records/Registration.hpp"
#include "../common/utils.hpp"
#include <botan/sha2_32.h>
#include <iostream>


std::shared_ptr<Record> HSProtocols::generateRecord()
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

/*
      const int IN  = 512;
      const int OUT = IN * 16;

      uint8_t arr[OUT];
      memset(arr, 0, OUT);
      for (int j = 0; j < IN; j++)
      {
         Botan::SHA_256 sha256;
         uint8_t hashBin[32];
         auto hashRaw = sha256.process(std::to_string(j));
         memcpy(hashBin, hashRaw, 32);

         auto dst = Utils::arrayToUInt32(hashBin);
         arr[dst % OUT]++;
      }

      for (int j = 0; j < OUT; j++)
         if ((int)arr[j] > 1)
             std::cout << (int)arr[j] << ",";
      std::cout << std::endl;
*/
}
