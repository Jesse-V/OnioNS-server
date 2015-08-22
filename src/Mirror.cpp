
#include "Mirror.hpp"
#include "tcp/Server.hpp"
#include <onions-common/tcp/TorStream.hpp>
#include <onions-common/containers/Cache.hpp>
#include <onions-common/Common.hpp>
#include <onions-common/Log.hpp>
#include <onions-common/Config.hpp>
#include <onions-common/Constants.hpp>
#include <botan/pubkey.h>
#include <thread>
#include <fstream>
#include <iostream>

typedef boost::exception_detail::clone_impl<
    boost::exception_detail::error_info_injector<boost::system::system_error>>
    BoostSystemError;


// definitions for static variables
std::vector<boost::shared_ptr<Session>> Mirror::subscribers_;
boost::shared_ptr<Session> Mirror::authSession_;
std::shared_ptr<boost::asio::io_service> Mirror::authIO_;


void Mirror::startServer(bool isQNode)
{
  loadCache();

  if (isQNode)
    Log::get().notice("Running as a Quorum server.");
  else
    Log::get().notice("Running as normal server.");

  // auto mt = std::make_shared<MerkleTree>(Cache::get().getSortedList());

  try
  {
    if (!isQNode)
      subscribeToQuorum();

    Server s(isQNode);
    s.start();
  }
  catch (const BoostSystemError& ex)
  {
    Log::get().error(ex.what());
  }
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



void Mirror::addSubscriber(const boost::shared_ptr<Session>& session)
{
  subscribers_.push_back(session);
}



void Mirror::broadcastEvent(const std::string& type, const Json::Value& value)
{
  Json::Value event;
  event["type"] = type;
  event["value"] = value;

  for (auto s : subscribers_)
    s->asyncWrite(event);

  Log::get().notice("Broadcasted to " + std::to_string(subscribers_.size()) +
                    " subscribers.");
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
    Log::get().error("Failed to parse cache!");

  // interpret JSON as Records and load into cache
  Log::get().notice("Preparing Records... ");
  for (uint n = 0; n < cacheValue.size(); n++)
    if (!Cache::add(Common::parseRecord(cacheValue[n])))
      Log::get().error("Invalid Record inside cache!");
}



void Mirror::subscribeToQuorum()
{
  std::thread t(receiveEvents);
  t.detach();
}



void Mirror::receiveEvents()
{
  const static int RECONNECT_DELAY = 10;
  authIO_ = std::make_shared<boost::asio::io_service>();
  auto nodeConfig = Config::getQuorumNode()[0];

  while (true)  // reestablish lost network connection
  {
    TorStream mirror(*authIO_, "127.0.0.1", 9050, nodeConfig["addr"].asString(),
                     Const::SERVER_PORT);
    Session session(*authIO_, -1);

    Log::get().notice("Connected! Subscribing to events...");
    session.asyncWrite("subscribe", "");
    authIO_->run();

    Log::get().warn("Lost connection to Quorum server.");
    std::this_thread::sleep_for(std::chrono::seconds(RECONNECT_DELAY));
  }
}
