
#include "Mirror.hpp"
#include "tcp/Server.hpp"
#include <onions-common/containers/Cache.hpp>
#include <onions-common/Common.hpp>
#include <onions-common/Log.hpp>
#include <onions-common/Config.hpp>
#include <onions-common/Constants.hpp>
#include <onions-common/crypto/ed25519.h>
#include <botan/base64.h>
#include <botan/pubkey.h>
#include <boost/make_shared.hpp>
#include <thread>
#include <fstream>
#include <pwd.h>

typedef boost::exception_detail::clone_impl<
    boost::exception_detail::error_info_injector<boost::system::system_error>>
    BoostSystemError;


// definitions for static variables
std::vector<boost::shared_ptr<Session>> Mirror::subscribers_;
boost::shared_ptr<Session> Mirror::authSession_;
std::shared_ptr<Page> Mirror::page_;


void Mirror::startServer(const std::string& bindIP,
                         ushort socksPort,
                         bool isQNode)
{
  resumeState();

  if (isQNode)
    Log::get().notice("Running as a Quorum server.");
  else
    Log::get().notice("Running as normal server.");

  // auto mt = std::make_shared<MerkleTree>(Cache::get().getSortedList());

  try
  {
    if (!isQNode)
      subscribeToQuorum(socksPort);

    Server s(bindIP, isQNode);
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



void Mirror::resumeState()
{
  loadPages();

  /*
    // interpret JSON as Records and load into cache
    Log::get().notice("Preparing Records... ");
    for (uint n = 0; n < cacheValue.size(); n++)
      if (!Cache::add(Common::parseRecord(cacheValue[n])))
        Log::get().error("Invalid Record inside cache!");*/
}



// generates a key and saves the private half if one does not exist
ED_KEY Mirror::getPublicKey()
{
  ed25519_secret_key sk;

  Log::get().notice("Loading Ed25519 key...");
  std::string homeDir(getpwuid(getuid())->pw_dir);

  // load private key from file, or generate and save a new one
  std::ifstream keyFile;

  keyFile.open(homeDir + "/ed25519.key", std::fstream::in);
  if (keyFile.is_open())
  {
    Json::Value obj;
    keyFile >> obj;
    Botan::base64_decode(sk, obj["key"].asString(), false);

    Log::get().notice("Ed25519 key successfully loaded.");
  }
  else
  {
    Log::get().notice("Keyfile does not exist. Generating new key...");

    Botan::AutoSeeded_RNG rng;
    rng.randomize(sk, Const::ED25519_KEY_LEN);

    Json::Value obj;
    obj["key"] = Botan::base64_encode(sk, Const::ED25519_KEY_LEN);

    Json::FastWriter writer;
    std::fstream keyOutFile(homeDir + "/ed25519.key", std::fstream::out);
    keyOutFile << writer.write(obj);
    keyOutFile.close();

    Log::get().notice("Ed25519 key successfully saved to disk.");
  }

  ed25519_public_key pk;
  ed25519_publickey(sk, pk);

  ED_KEY array;
  memcpy(array.data(), pk, Const::ED25519_KEY_LEN);
  return array;
}



void Mirror::loadPages()
{
  Log::get().notice("Loading Pagechain from file...");
  std::string homeDir(getpwuid(getuid())->pw_dir);

  std::ifstream pagechainFile;
  pagechainFile.open(homeDir + "/pagechain.json", std::fstream::in);
  if (pagechainFile.is_open())
  {
    Json::Value obj;
    pagechainFile >> obj;
    page_ = std::make_shared<Page>(obj);
    // todo: assert that loaded key matches the key in pagechain.json

    Log::get().notice("Pagechain successfully loaded.");
  }
  else
  {
    Log::get().warn("Pagechain file does not exist.");

    SHA384_HASH latestRandom = {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7};  // todo
    ED_KEY pk = getPublicKey();
    page_ = std::make_shared<Page>(latestRandom, pk);

    std::fstream outFile(homeDir + "/pagechain.json", std::fstream::out);
    outFile << page_->toString();
    outFile.close();

    Log::get().notice("Blank Page successfully saved to disk.");
  }
}



void Mirror::subscribeToQuorum(ushort socksPort)
{
  std::thread t(std::bind(&Mirror::receiveEvents, socksPort));
  t.detach();
}



void Mirror::receiveEvents(ushort socksPort)
{
  const static int RECONNECT_DELAY = 10;
  const auto QNODE = Config::getQuorumNode()[0];

  while (true)  // reestablish lost network connection
  {
    try
    {
      TorStream torStream("127.0.0.1", 9050, QNODE["addr"].asString(),
                          Const::SERVER_PORT);

      Log::get().notice("Subscribing to events...");
      torStream.getIO().reset();  // reset for new asynchronous calls
      authSession_ = boost::make_shared<Session>(torStream.getSocket(), -1);
      authSession_->asyncWrite("subscribe", "");
      torStream.getIO().run();  // run asynchronous calls
    }
    catch (const BoostSystemError& ex)
    {
      Log::get().warn("Connection error, " + std::string(ex.what()));
      std::this_thread::sleep_for(std::chrono::seconds(RECONNECT_DELAY));
      continue;
    }

    Log::get().warn("Lost connection to Quorum server.");
    std::this_thread::sleep_for(std::chrono::seconds(RECONNECT_DELAY));
  }
}
