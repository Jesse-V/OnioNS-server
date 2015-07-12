
#include "Mirror.hpp"
#include "tcp/Server.hpp"
#include <onions-common/containers/Cache.hpp>
#include <onions-common/Common.hpp>
#include <onions-common/Log.hpp>
#include <onions-common/Constants.hpp>
#include <botan/pubkey.h>
#include <fstream>
#include <iostream>

std::vector<std::shared_ptr<Session>> Mirror::connections_;


void Mirror::startServer()
{
  loadCache();

  // auto mt = std::make_shared<MerkleTree>(Cache::get().getSortedList());

  Server s(10053);
  s.start();
}



UInt8Array Mirror::signMerkleRoot(Botan::RSA_PrivateKey* key,
                                  const MerkleTreePtr& mt)
{
  static Botan::AutoSeeded_RNG rng;

  Botan::PK_Signer signer(*key, "EMSA-PSS(SHA-384)");
  auto sig = signer.sign_message(mt->getRoot(), Const::SHA384_LEN, rng);
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



void Mirror::loadCache()
{
  Log::get().notice("Loading Record cache... ");
  std::ifstream cacheFile;
  cacheFile.open("/var/lib/tor-onions/cache.json", std::fstream::in);
  if (!cacheFile.is_open())
    Log::get().error("Cannot open cache!");

  // parse cache file into JSON object
  Json::Value cacheValue;
  Json::Reader reader;
  std::string json((std::istreambuf_iterator<char>(cacheFile)),
                   std::istreambuf_iterator<char>());
  if (!reader.parse(json, cacheValue))
  {
    Log::get().error("Failed to parse cache!");
    return;
  }

  // interpret JSON as Records and load into cache
  Log::get().notice("Preparing Records... ");
  for (uint n = 0; n < cacheValue.size(); n++)
    if (!Cache::add(Common::parseRecord(cacheValue[n])))
      Log::get().error("Invalid Record inside cache!");
}
