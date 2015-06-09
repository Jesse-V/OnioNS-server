
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

  Server s(Env::SERVER_PORT);
  s.start();
}



UInt8Array Mirror::signMerkleRoot(Botan::RSA_PrivateKey* key,
                                  const MerkleTreePtr& mt) const
{
  static Botan::AutoSeeded_RNG rng;

  Botan::PK_Signer signer(*key, "EMSA-PSS(SHA-384)");
  auto sig = signer.sign_message(mt->getRoot(), Env::SHA384_LEN, rng);
  uint8_t* bin = new uint8_t[sig.size()];
  memcpy(bin, sig, sig.size());
  return std::make_pair(bin, sig.size());
}



void Mirror::addConnection(const std::shared_ptr<Session>& session)
{
  connections_.push_back(session);
}



void Mirror::broadcastEvent(const std::string& type, const Json::Value& value)
{
  Json::Value event;
  event["type"] = type;
  event["value"] = value;

  for (auto s : connections_)
    if (s->isSubscriber())
      s->asyncWrite(event);
}



void Mirror::loadCache() const
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
