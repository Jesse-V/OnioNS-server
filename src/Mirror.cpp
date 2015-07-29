
#include "Mirror.hpp"
#include "tcp/Server.hpp"
#include <onions-common/containers/Cache.hpp>
#include <onions-common/Common.hpp>
#include <onions-common/Log.hpp>
#include <onions-common/Config.hpp>
#include <onions-common/Constants.hpp>
#include <botan/pubkey.h>
#include <fstream>
#include <iostream>

// definitions for static variables
std::vector<std::shared_ptr<Session>> Mirror::connections_;
boost::asio::io_service Mirror::io_service;
boost::asio::ip::tcp::socket Mirror::socket_(io_service);


void Mirror::startServer(const std::string& host, ushort port, bool isAuthority)
{
  std::cout << "*" << host << "*" << std::endl;
  loadCache();

  if (isAuthority)
    Log::get().notice("Running as authoritative server.");
  else
    Log::get().notice("Running as normal server.");

  // auto mt = std::make_shared<MerkleTree>(Cache::get().getSortedList());

  if (!isAuthority)
  {
    auto addr = Config::getAuthority()[0];  // addr["ip"].asString()
    openSocketTo("127.0.0.1", addr["port"].asInt());
    auto rStr = serverSendReceive("subscribe", "");
    Log::get().notice("Authority response: " + rStr);
  }

  try
  {
    Server s(host, port, isAuthority);
    s.start();
  }
  catch (boost::exception_detail::clone_impl<
      boost::exception_detail::error_info_injector<
          boost::system::system_error>> const& ex)
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



void Mirror::openSocketTo(const std::string& host, ushort port)
{
  using boost::asio::ip::tcp;

  Log::get().notice("Connecting to authority server... ");

  tcp::resolver resolver(io_service);
  tcp::resolver::query query(host, std::to_string(port));
  tcp::resolver::iterator iterator = resolver.resolve(query);
  boost::asio::connect(socket_, iterator);

  Log::get().notice("Connected!");
}



std::string Mirror::serverSendReceive(const std::string& type,
                                      const std::string& msg)
{  // similar to -common's SocksClient::sendReceive

  Log::get().notice("Server-server send... ");

  // send as JSON
  Json::Value outVal;
  outVal["command"] = type;
  outVal["value"] = msg;
  Json::FastWriter writer;
  boost::asio::write(socket_, boost::asio::buffer(writer.write(outVal)));

  // read from socket until newline
  Log::get().notice("Server-server receive... ");
  boost::asio::streambuf response;
  boost::asio::read_until(socket_, response, "\n");

  // convert to string
  std::string responseStr;
  std::istream is(&response);
  is >> responseStr;

  // parse into JSON object
  Json::Reader reader;
  Json::Value responseVal;
  if (!reader.parse(responseStr, responseVal))
  {
    Log::get().warn("Failed to parse response from server.");
    return "failure";
  }

  if (!responseVal.isMember("error") && !responseVal.isMember("response"))
  {
    Log::get().warn("Invalid response from server.");
    return "failure";
  }

  Log::get().notice("Server-server communication complete.");

  return responseVal["response"].asString();
}
