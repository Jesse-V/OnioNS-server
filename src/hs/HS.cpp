
#include "HS.hpp"
#include "../common/Common.hpp"
#include "../common/records/CreateR.hpp"
#include "../common/utils.hpp"
#include "../Flags.hpp"
#include <iostream>


std::shared_ptr<Record> HS::createRecord()
{
  try
  {
    auto r = std::make_shared<CreateR>(loadKey(), Flags::get().getDomainName(),
                                       "AD97364FC20BEC80");
    NameList list = r->getNameList();
    list.push_back(
        std::make_pair("sub." + Flags::get().getDomainName(), "example.tor"));
    r->setNameList(list);

    std::cout << std::endl;
    auto json = r->asJSON();
    std::cout << "Initial Record: (" << json.length() << " bytes) \n" << json
              << std::endl;

    r->makeValid(4);

    std::cout << std::endl;
    std::cout << "Result:" << std::endl;
    std::cout << *r << std::endl;

    std::cout << std::endl;
    json = r->asJSON();
    std::cout << "Final Record, ready for transmission: (" << json.length()
              << " bytes) \n" << json << std::endl;

    return r;
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << "\n";
  }

  return NULL;
}



// ***************************** PRIVATE METHODS *****************************



Botan::RSA_PrivateKey* HS::loadKey()
{
  std::cout << "Opening HS key... ";

  Botan::AutoSeeded_RNG rng;
  Botan::RSA_PrivateKey* rsaKey =
      Utils::loadKey(Flags::get().getKeyPath(), rng);
  if (rsaKey != NULL)
    std::cout << "done." << std::endl;

  return rsaKey;
}
