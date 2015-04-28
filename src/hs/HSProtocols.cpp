
#include "HSProtocols.hpp"
#include "../common/CommonProtocols.hpp"
#include "../common/records/CreateR.hpp"
#include "../common/utils.hpp"
#include <iostream>


std::shared_ptr<Record> HSProtocols::createRecord()
{
   try
   {
      auto rsaKey = loadKey();
      auto hash384 = CommonProtocols::get().computeConsensusHash();

      auto r = std::make_shared<CreateR>(rsaKey, hash384,
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



// ***************************** PRIVATE METHODS *****************************



Botan::RSA_PrivateKey* HSProtocols::loadKey()
{
   std::cout << "Opening HS key... ";

   Botan::AutoSeeded_RNG rng;
   Botan::RSA_PrivateKey* rsaKey = Utils::loadKey("assets/example.key", rng);
   if (rsaKey != NULL)
      std::cout << "done." << std::endl;

   return rsaKey;
}
