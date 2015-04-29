
#include "HSProtocols.hpp"
#include "../common/CommonProtocols.hpp"
#include "../common/records/CreateR.hpp"
#include "../common/utils.hpp"
#include <iostream>


std::shared_ptr<Record> HSProtocols::createRecord()
{
   try
   {
      auto r = std::make_shared<CreateR>(loadKey(), "example2.tor", "AD97364FC20BEC80");
      NameList list = r->getNameList();
      list.push_back(std::make_pair("sub.example2.tor", "example.tor"));
      r->setNameList(list);

      std::cout << std::endl;
      auto json = r->asJSON();
      std::cout << "Initial Record: (" << json.length() << " bytes) \n" <<
         json << std::endl;

      r->makeValid(4);

      std::cout << std::endl;
      std::cout << "Result:" << std::endl;
      std::cout << *r << std::endl;

      std::cout << std::endl;
      json = r->asJSON();
      std::cout << "Final Record, ready for transmission: (" <<
         json.length() << " bytes) \n" << json << std::endl;

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
