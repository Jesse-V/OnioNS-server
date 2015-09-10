
#include "Mirror.hpp"
#include "tcp/Server.hpp"
#include <onions-common/containers/Cache.hpp>
#include <onions-common/Common.hpp>
#include <onions-common/Log.hpp>
#include <onions-common/Config.hpp>
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
std::shared_ptr<MerkleTree> Mirror::merkleTree_;
std::pair<ED_KEY, ED_KEY> Mirror::keypair_;
bool Mirror::isQuorumNode_;


void Mirror::startServer(const std::string& bindIP,
                         ushort socksPort,
                         bool isQNode)
{
  resumeState();
  isQuorumNode_ = isQNode;

  if (isQuorumNode_)
    Log::get().notice("Running as a Quorum server.");
  else
    Log::get().notice("Running as normal server.");

  // auto mt = std::make_shared<MerkleTree>(Cache::get().getSortedList());

  try
  {
    if (!isQuorumNode_)
      subscribeToQuorum(socksPort);

    Server s(bindIP);
    s.start();
  }
  catch (const BoostSystemError& ex)
  {
    Log::get().error(ex.what());
  }
}



void Mirror::addSubscriber(const boost::shared_ptr<Session>& session)
{
  subscribers_.push_back(session);
}



bool Mirror::processNewRecord(const RecordPtr& record)
{
  if (!Cache::add(record))
    return false;  // if already exists in cache

  page_->addRecord(record);

  Log::get().notice("Broadcasting to " + std::to_string(subscribers_.size()) +
                    " subscribers...");

  // send the Record
  Json::Value rEvent;
  rEvent["type"] = "upload";
  rEvent["value"] = record->asJSON();
  for (auto s : subscribers_)
    s->asyncWrite(rEvent);

  if (isQuorumNode_)
  {  // regenerate merkle tree, sign it, and rebroadcast signatures
    merkleTree_ = std::make_shared<MerkleTree>(Cache::getSortedList());
    auto sigEvent = getRootSignature();
    for (auto s : subscribers_)
      s->asyncWrite(sigEvent);
  }

  Log::get().notice("Transmission complete.");
  return true;
}



ED_SIGNATURE Mirror::signMerkleRoot()
{
  ED_SIGNATURE signature;
  ed25519_sign(merkleTree_->getRoot().data(), Const::SHA384_LEN,
               keypair_.first.data(), keypair_.second.data(), signature.data());
  return signature;
}



Json::Value Mirror::getRootSignature()
{
  // assemble signatures
  Json::Value sigObj;
  ED_SIGNATURE edSig = signMerkleRoot();
  auto sinceEpoch =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch()).count();
  sigObj["signature"] = Botan::base64_encode(edSig.data(), edSig.size());
  sigObj["timestamp"] = std::to_string(sinceEpoch);
  sigObj["count"] = std::to_string(Cache::getRecordCount());

  // send the signatures
  Json::Value sigEvent;
  sigEvent["type"] = "merkleSignature";
  sigEvent["value"] = sigObj;
  return sigEvent;
}



// ***************************** PRIVATE METHODS *****************************



void Mirror::resumeState()
{
  Log::get().notice("Resuming state... ");

  loadKeyPair();
  loadPages();
  Cache::add(page_->getRecords());

  Log::get().notice("State successfully resumed.");
}



void Mirror::loadPages()
{
  Log::get().notice("Loading Pagechain from file...");
  std::string workingDir(getpwuid(getuid())->pw_dir);
  workingDir += "/.OnioNS/";

  std::ifstream pagechainFile;
  pagechainFile.open(workingDir + "pagechain.json", std::fstream::in);
  if (pagechainFile.is_open())
  {
    Json::Value obj;
    pagechainFile >> obj;
    page_ = std::make_shared<Page>(obj);
    if (!std::equal(keypair_.second.begin(), keypair_.second.end(),
                    page_->getOwnerPublicKey().data()))
      Log::get().error("Mismatched Page public key, does not match keyfile.");

    Log::get().notice("Pagechain successfully loaded.");
  }
  else
  {
    Log::get().warn("Pagechain file does not exist.");

    SHA384_HASH latestRandom = {7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7};  // todo

    page_ = std::make_shared<Page>(latestRandom, keypair_.second);

    mkdir(workingDir.c_str(), 0755);
    std::fstream outFile(workingDir + "pagechain.json", std::fstream::out);
    outFile << page_->toString();
    outFile.close();

    Log::get().notice("Blank Page successfully saved to disk.");
  }
}



void Mirror::loadKeyPair()
{
  Log::get().notice("Loading Ed25519 key...");
  std::string workingDir(getpwuid(getuid())->pw_dir);
  workingDir += "/.OnioNS/";

  ED_KEY privateKey = loadSecretKey(workingDir);
  ED_KEY publicKey;
  ed25519_public_key pk;

  ed25519_publickey(privateKey.data(), pk);
  memcpy(publicKey.data(), pk, Const::ED25519_KEY_LEN);

  keypair_ = std::make_pair(privateKey, publicKey);

  Log::get().notice("Server public key: " +
                    Botan::base64_encode(pk, Const::ED25519_KEY_LEN));
}



ED_KEY Mirror::loadSecretKey(const std::string& workingDir)
{  // load private key from file, or generate and save a new one

  ED_KEY sk;

  std::ifstream keyFile;
  keyFile.open(workingDir + "ed25519.key", std::fstream::in);
  if (keyFile.is_open())
  {
    Json::Value obj;
    keyFile >> obj;
    if (Botan::base64_decode(sk.data(), obj["key"].asString()) != sk.size())
      Log::get().error("Error decoding Ed25519 keyfile: invalid size.");

    Log::get().notice("Ed25519 key successfully loaded.");
  }
  else
  {
    Log::get().notice("Keyfile does not exist. Generating new key...");

    Botan::AutoSeeded_RNG rng;
    rng.randomize(sk.data(), Const::ED25519_KEY_LEN);

    Json::Value obj;
    obj["key"] = Botan::base64_encode(sk.data(), Const::ED25519_KEY_LEN);

    Json::FastWriter writer;
    mkdir(workingDir.c_str(), 0755);
    std::fstream keyOutFile(workingDir + "ed25519.key", std::fstream::out);
    keyOutFile << writer.write(obj);
    keyOutFile.close();

    Log::get().notice("Ed25519 key successfully saved to disk.");
  }

  return sk;
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
      TorStream torStream("127.0.0.1", socksPort, QNODE["addr"].asString(),
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
