
#include "HS.hpp"
#include "../common/Common.hpp"
#include "../common/records/CreateR.hpp"
#include "../common/tcp/SocksClient.hpp"
#include "../common/utils.hpp"
#include "../common/Environment.hpp"
#include "../Flags.hpp"
#include <iostream>


RecordPtr HS::createRecord() const
{
  auto r = promptForRecord();

  std::cout << "\n" << *r << "\n" << std::endl;
  std::cout << "Record prepared. Hit Enter to begin making it valid. ";
  std::string tmp;
  std::getline(std::cin, tmp);

  r->makeValid(4);

  std::cout << std::endl;
  std::cout << *r << std::endl;

  std::cout << std::endl;
  auto json = r->asJSON();
  std::cout << "Final Record is " << json.length()
            << " bytes, ready for transmission: \n\n" << json << std::endl;

  return r;
}



RecordPtr HS::promptForRecord() const
{
  std::cout
      << "Here you can claim a domain name and multiple subdomains for your"
         " hidden service. They can point to either a .tor or a .onion domain,"
         " keeping it all within Tor. For example, you may claim"
         " \"example.tor\" -> \"onions55e7yam27n.onion\", \"foo.example.tor\""
         " -> \"foo.tor\", \"bar.example.tor\" -> \"bar.tor\", and"
         " \"a.b.c.example.tor\" -> \"3g2upl4pq6kufc4m.onion\"."
         " The association between these names and your hidden service is"
         " cryptographically locked and made valid after some computational"
         " work. This work will follow the prompts for the domains. \n";

  std::string name;
  std::cout << "The primary domain name must end in \".tor\"" << std::endl;
  while (name.length() < 5 || !Utils::strEndsWith(name, ".tor"))
  {
    std::cout << "The primary domain name: ";
    std::getline(std::cin, name);
  }

  std::string pgp;
  std::cout << "You may optionally supply your PGP fingerprint, \n"
               "which must be a power of two in length." << std::endl;
  while (!Utils::isPowerOfTwo(pgp.length()))
  {
    std::cout << "Your PGP fingerprint: ";
    std::getline(std::cin, pgp);  //"AD97364FC20BEC80"
  }

  std::cout
      << "You may provide up to 24 subdomain-destination pairs.\n"
         "Just provide the labels before the primary domain name. For example,"
         " to claim \"foo.example.tor\" -> \"bar.tor\", simply provide \"foo\" "
         "and"
         " then \"bar.tor\". Leave the subdomain blank when finished."
      << std::endl;

  NameList list;
  for (int n = 1; n <= 24; n++)
  {
    std::string src = name, dest;

    while (Utils::strEndsWith(src, name))
    {
      std::cout << "Subdomain " << n << ": ";
      std::getline(std::cin, src);
    }

    if (src.length() == 0)
      break;

    while ((!Utils::strEndsWith(dest, ".tor") &&
            !Utils::strEndsWith(dest, ".onion")) ||
           (Utils::strEndsWith(dest, ".onion") && dest.length() != (16 + 6)))
    {
      std::cout << "   Destination: ";
      std::getline(std::cin, dest);
    }

    list.push_back(std::make_pair(src, dest));
  }

  std::cout << std::endl;
  auto r = std::make_shared<CreateR>(loadKey(), name, pgp);
  r->setSubdomains(list);
  return r;
}



bool HS::sendRecord(const RecordPtr& r) const
{
  auto socks = SocksClient::getCircuitTo(Env::get().getMirrorIP());
  if (!socks)
    throw std::runtime_error("Unable to connect!");

  std::cout << "Uploading Record..." << std::endl;
  auto received = socks->sendReceive("upload", r->asJSON());
  if (received.isMember("error"))
  {
    std::cerr << "Err: " << received["error"].asString() << std::endl;
    return false;
  }

  return true;
}



// ***************************** PRIVATE METHODS *****************************



Botan::RSA_PrivateKey* HS::loadKey() const
{
  std::cout << "Opening HS key... ";

  Botan::AutoSeeded_RNG rng;
  Botan::RSA_PrivateKey* rsaKey =
      Utils::loadKey(Flags::get().getKeyPath(), rng);
  if (rsaKey != NULL)
    std::cout << "done." << std::endl;

  return rsaKey;
}
