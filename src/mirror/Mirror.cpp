
#include "Mirror.hpp"
#include "tcp/Server.hpp"
#include "containers/Cache.hpp"
#include "../common/Common.hpp"
#include <botan/pubkey.h>
#include <fstream>
#include <iostream>


void Mirror::startServer()
{
  loadCache();

  // auto mt = std::make_shared<MerkleTree>(Cache::get().getSortedList());

  Server s(Environment::SERVER_PORT);
  s.start();
}



void Mirror::signMerkleRoot(Botan::RSA_PrivateKey* key, const MerkleTreePtr& mt)
{
  static Botan::AutoSeeded_RNG rng;

  Botan::PK_Signer signer(*key, "EMSA-PSS(SHA-384)");
  auto sig = signer.sign_message(mt->getRoot(), Environment::SHA384_LEN, rng);
}



void Mirror::loadCache()
{
  std::cout << "Loading Record cache... " << std::endl;
  std::fstream cacheFile("/var/lib/tor-onions/cache.txt");
  if (!cacheFile)
    throw std::runtime_error("Cannot open cache!");

  // parse cache file into JSON object
  Json::Value cacheValue;
  Json::Reader reader;
  std::string json((std::istreambuf_iterator<char>(cacheFile)),
                   std::istreambuf_iterator<char>());
  if (!reader.parse(json, cacheValue))
  {
    std::cerr << "Failed to parse cache!" << std::endl;
    return;
  }

  // interpret JSON as Records and load into cache
  std::cout << "Preparing Records... " << std::endl;
  for (uint n = 0; n < cacheValue.size(); n++)
    if (!Cache::get().add(Common::get().parseRecord(cacheValue[n])))
      throw std::runtime_error("Invalid Record inside cache!");
}
