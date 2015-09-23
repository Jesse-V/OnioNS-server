
#include "Mirror.hpp"
#include "tcp/Server.hpp"
#include <onions-common/containers/Cache.hpp>
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



void Mirror::subscribeForRecords(const boost::shared_ptr<Session>& session)
{
  waitingForRecords_.push_back(session);
}



bool Mirror::processNewRecord(int sessionID, const RecordPtr& record)
{
  if (!Cache::add(record))
    return false;  // if already exists in cache

  page_->addRecord(record);
  tellSubscribers(record);

  merkleTree_ = std::make_shared<MerkleTree>(Cache::getSortedList());

  if (sessionID == qSession_->getID())
  {
    Log::get().notice("Record came from Quorum. Requesting signature.");
    qRootSig_ = fetchQuorumRootSignature();
  }

  return true;
}



void Mirror::tellSubscribers(const RecordPtr& record)
{
  Log::get().notice("Broadcasting Record to " +
                    std::to_string(waitingForRecords_.size()) + " servers...");

  // send the Record
  Json::Value rEvent;
  rEvent["type"] = "putRecord";
  rEvent["value"] = record->asJSON();
  for (auto session : waitingForRecords_)
    session->asyncWrite(rEvent);

  waitingForRecords_.clear();
  Log::get().notice("Broadcast complete.");
}



std::string Mirror::signTransmission(const Json::Value& value) const
{
  ED_SIGNATURE signature;
  std::string data = value["type"].asString() + value["value"].asString();
  ed25519_sign(reinterpret_cast<const uint8_t*>(data.c_str()), data.length(),
               keypair_.first.data(), keypair_.second.data(), signature.data());
  return Botan::base64_encode(signature.data(), signature.size());
}



Json::Value Mirror::getRootSignature() const
{
  // todo: if Q, return my own, otherwise archived

  Json::Value sigObj;
  sigObj["signature"] =
      isQuorumNode_ ? signMerkleRoot()
                    : Botan::base64_encode(qRootSig_.data(), qRootSig_.size());
  sigObj["count"] = std::to_string(Cache::getRecordCount());

  Json::Value sigEvent;
  sigEvent["type"] = "merkleSignature";
  sigEvent["value"] = sigObj;

  return sigEvent;
}



ED_SIGNATURE Mirror::fetchQuorumRootSignature()
{
  ED_SIGNATURE sig;

  auto response = qStream_->sendReceive("getRootSignature", "");
  if (response["type"] == "error")
  {
    Log::get().warn("Error when getting root signature from Quorum node: " +
                    response["value"].asString());
  }
  else
  {
    if (!response.isMember("signature") || !response.isMember("count"))
      Log::get().error("Invalid root signature response from server.");

    // check number of Records
    if (response["count"].asInt() != Cache::getRecordCount())
      Log::get().warn("Quorum has " + response["count"].asString() +
                      " records, we have " +
                      std::to_string(Cache::getRecordCount()));
    // todo: we are out of date

    // decode signature
    if (Botan::base64_decode(sig.data(), response["signature"].asString()) !=
        sig.size())
      Log::get().warn("Invalid root signature from Quorum node.");

    // get public key
    ED_KEY qPubKey;
    static auto Q_KEY = Config::getQuorumNode()[0]["key"].asString();
    std::copy(Q_KEY.begin(), Q_KEY.end(), qPubKey.data());

    // check signature
    int status =
        ed25519_sign_open(merkleTree_->getRoot().data(), Const::SHA384_LEN,
                          qPubKey.data(), sig.data());
    if (status == 0)
      Log::get().notice("Valid Ed25519 Quorum signature on root.");
    else if (status == 1)
      Log::get().warn("Invalid Ed25519 Quorum signature on root.");
    else
      Log::get().warn("General Ed25519 signature failure on root.");
  }

  return sig;
}


/*
// 1 for failure, 0 for success, -1 for crypto error, like sign_open
int Mirror::verifyRootSignature(const Json::Value& sigObj) const
{
  if (!sigObj.isMember("signature") || !sigObj.isMember("count"))
    return 1;

  if (sigObj["count"].asInt() != Cache::getRecordCount())
    return 1;  // todo: we are out of date

  ED_SIGNATURE sig;
  if (Botan::base64_decode(sig.data(), sigObj["signature"].asString()) !=
      sig.size())
    return 1;

  ED_KEY qPubKey;
  static auto Q_KEY = Config::getQuorumNode()[0]["key"].asString();
  std::copy(Q_KEY.begin(), Q_KEY.end(), qPubKey.data());

  return ed25519_sign_open(merkleTree_->getRoot().data(), Const::SHA384_LEN,
                           qPubKey.data(), sig.data());
}*/



std::string Mirror::signMerkleRoot() const
{
  ED_SIGNATURE signature;
  ed25519_sign(merkleTree_->getRoot().data(), Const::SHA384_LEN,
               keypair_.first.data(), keypair_.second.data(), signature.data());
  return Botan::base64_encode(signature.data(), signature.size());
}



std::string Mirror::getWorkingDir()
{
  std::string workingDir(getpwuid(getuid())->pw_dir);
  workingDir += "/.OnioNS/";

  if (mkdir(workingDir.c_str(), 0750) == 0)
    Log::get().notice("Working directory successfully created.");

  return workingDir;
}



std::shared_ptr<MerkleTree> Mirror::getMerkleTree() const
{
  return merkleTree_;
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

  std::ifstream pageFile;
  pageFile.open(getWorkingDir() + "pagechain.json", std::fstream::in);
  if (pageFile.is_open())
  {
    Json::Value obj;
    pageFile >> obj;
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

    std::fstream outFile(getWorkingDir() + "pagechain.json", std::fstream::out);
    outFile << page_->toString();
    outFile.close();

    Log::get().notice("Blank Page successfully saved to disk.");
  }
}



void Mirror::loadKeyPair()
{
  Log::get().notice("Loading Ed25519 key...");

  ED_KEY privateKey = loadSecretKey(getWorkingDir());
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
    std::fstream keyOutFile(workingDir + "ed25519.key", std::fstream::out);
    keyOutFile << writer.write(obj);
    keyOutFile.close();

    Log::get().notice("Ed25519 key successfully saved to disk.");
  }

  return sk;
}



void Mirror::subscribeToQuorum(ushort socksPort)
{
  const static int RECONNECT_DELAY = 10;
  const static auto REMOTE_PORT = Const::SERVER_PORT;
  const auto Q_NODE = Config::getQuorumNode()[0];
  const auto Q_ONION = Q_NODE["addr"].asString();
  const auto Q_KEY = Q_NODE["key"].asString();

  ED_KEY qPubKey;
  std::copy(Q_KEY.begin(), Q_KEY.end(), qPubKey.data());

  while (true)  // reestablish lost network connection
  {
    try
    {
      qStream_ = std::make_shared<AuthenticatedStream>(
          "127.0.0.1", socksPort, Q_ONION, REMOTE_PORT, qPubKey);
      // todo, are we even using authStream?
      // we are just using TorStream to tunnel through SOCKS5!

      Log::get().notice("Subscribing to events...");
      qStream_->getIO().reset();  // reset for new asynchronous calls
      qSession_ = boost::make_shared<Session>(qStream_->getSocket(), -1);
      qSession_->asyncWrite("waitForRecord", "");  // this calls asyncRead
      qStream_->getIO().run();                     // run asynchronous calls
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



  /*
  std::thread t(std::bind(
      [&](ushort sp)
      {
        Mirror::get().receiveEvents(sp);
      },
      socksPort));
  t.detach();*/
}


/*
void Mirror::receiveEvents(ushort socksPort)
{
  const static int RECONNECT_DELAY = 10;
  const auto Q_NODE = Config::getQuorumNode()[0];
  const auto Q_ONION = Q_NODE["addr"].asString();
  const auto Q_KEY = Q_NODE["key"].asString();

  ED_KEY qPubKey;
  std::copy(Q_KEY.begin(), Q_KEY.end(), qPubKey.data());

  while (true)  // reestablish lost network connection
  {
    try
    {
      AuthenticatedStream authStream("127.0.0.1", socksPort, Q_ONION,
                                     Const::SERVER_PORT, qPubKey);
      // todo, are we even using authStream?
      // we are just using TorStream to tunnel through SOCKS5!

      Log::get().notice("Subscribing to events...");
      authStream.getIO().reset();  // reset for new asynchronous calls
      authSession_ = boost::make_shared<Session>(authStream.getSocket(), -1);
      authSession_->asyncWrite("subscribe", "");  // this calls asyncRead
      authStream.getIO().run();                   // run asynchronous calls
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
*/
