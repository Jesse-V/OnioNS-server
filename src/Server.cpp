
#include "Server.hpp"
#include <onions-common/Log.hpp>
#include <onions-common/Common.hpp>
#include <onions-common/Constants.hpp>
#include <onions-common/tor_ipc/SocketException.hpp>
#include <onions-common/CyoEncode/CyoEncode.hpp>
#include <onions-common/CyoEncode/CyoDecode.hpp>
#include <botan/base64.h>
#include <algorithm>


Server::Server(jsonrpc::AbstractServerConnector& connector,
               short socksPort,
               short controlPort,
               bool isQuorumNode)
    : NetworkServerInterface(connector)
{
  if (isQuorumNode)
    Log::get().notice("Running as a Quorum server.");
  else
    Log::get().notice("Running as normal server.");

  try
  {
    TorController control("127.0.0.1", controlPort);
    control.authenticateToTor(Common::getWorkingDirectory() +
                              "control.authcookie");
    control.waitForBootstrap();

    std::string hostname = getHostname();
    Log::get().notice("Server is reachable at " + hostname + ":" + std::to_string(Const::SERVER_PORT));

    SHA256_HASH hash;
    memset(hash.data(), 0, Const::SHA256_LEN);
    updateContactData(control, false, hash, hostname);
    Log::get().notice("Announced server availability via Contact field.");


/*
    SHA256_HASH hash;
    memset(hash.data(), 0, Const::SHA256_LEN);
    updateContactData(control, false, hash, "3g2upl4pq6kufc4m.onion");
    Log::get().notice("contact: \"" + control.getSetting("ContactInfo") + "\"");

    ContactData cd;
    auto data = getContactData(control.getSetting("ContactInfo"), cd);
    Log::get().notice(std::string("cd: ") + (cd.onDebugNetwork_ ? "true" : "false") + " - " + cd.addr_);
*/

    // shutting down: control.reloadSettings(); // reload from file

/*
    auto socket = control.getSocket();
    *socket << "GETCONF HiddenServiceOptions\r\n";
    std::string resp;
    *socket >> resp;
Log::get().notice("response: \"" + resp + "\"");
*/
    /*
        auto socket = control.getSocket();
        *socket << "GETCONF Contact\r\n";
        // GETINFO network-status
        // ns/all
        // status/fresh-relay-descs
        //

        std::string resp;
        *socket >> resp;
    Log::get().notice("contact: \"" + contact + "\"");
    */
  }
  catch (SocketException& e)
  {
    Log::get().warn("Communication error with Tor! " + e.description());
  }

  /*
  todo:
    getConsensusDocument stuff
    read ed25519 key?
  */
}



void Server::commitTicket(const std::string& commit, int version)
{
}



void Server::revealTicket(const Json::Value& record, int version)
{
}



void Server::putRecord(const Json::Value& record, int version)
{
}



Json::Value Server::getRecordWithSubtree(const std::string& domain, int version)
{
  Json::Value v;
  return v;
}



std::string Server::getDomain(const std::string& domain, int version)
{
  return "address.onion";
}



Json::Value Server::getRecordsSince(int time, int version)
{
  Json::Value v;
  return v;
}



// arguments: controller, <true/false>, hash, <address>.onion
bool Server::updateContactData(TorController& control, bool testNetwork, const SHA256_HASH& rootHash, const std::string& onionAddr)
{
  //initialize binary
  uint8_t bin[BIN_SIZE];
  memset(bin, 0, BIN_SIZE);

  // add root hash and onion address
  memcpy(bin, rootHash.data(), Const::SHA256_LEN);
  CyoDecode::Base32::Decode(&bin[Const::SHA256_LEN], onionAddr.c_str(), 16);
  return updateContactString(control, (testNetwork ? "1" : "0") + Botan::base64_encode(bin, BIN_SIZE));
}



bool Server::updateContactString(TorController& control, const std::string& str)
{
  control.reloadSettings(); // reload from file
  std::string origContact = control.getSetting("ContactInfo");

  auto pos = origContact.find("=");
  origContact = origContact.substr(pos + 1);
  std::string newContact = origContact + " |" + str;
  return control.setSetting("ContactInfo", "\"" + newContact + "\"");
}



bool Server::getContactData(const std::string& contactStr, Server::ContactData& cd)
{
  // check for OnioNS delimiter and the proper length
  std::size_t pos = contactStr.find_last_of('|');
  if (pos == std::string::npos || contactStr.size() - pos != 58)
    return false;

  // get flag and confirm its validity
  char debug = contactStr[pos + 1];
  if (debug != '1' && debug != '0')
  {
    Log::get().warn("Unexpected Contact flag! \"" + contactStr + "\"");
    return false;
  }

  // decode base64 and confirm validity
  uint8_t bin[BIN_SIZE]; // safe as I already check the length
  memset(bin, 0, BIN_SIZE);
  auto decodedSize = Botan::base64_decode(bin, contactStr.substr(pos + 2));
  if (decodedSize != BIN_SIZE)
  {
    Log::get().warn("Unusual base64 length! \"" + contactStr + "\"");
    return false;
  }

  // set debug and root hash variables
  cd.onDebugNetwork_ = debug == '1';
  memcpy(cd.rootHash_.data(), bin, Const::SHA256_LEN);

  // set hidden service address variable, convert to lowercase
  char hs_addr[HS_ADDR_BYTES];
  CyoEncode::Base32::Encode(hs_addr, &bin[Const::SHA256_LEN], HS_ADDR_BYTES);
  cd.addr_ = std::string(hs_addr) + ".onion";
  std::transform(cd.addr_.begin(), cd.addr_.end(), cd.addr_.begin(), ::tolower);

  return true;
}



std::string Server::getHostname()
{
  std::ifstream file(Common::getWorkingDirectory() + "hostname");
  if (!file)
    Log::get().error("Unable to open hostname file!");

  std::string hostname;
  file >> hostname;

  if (hostname.size() != 22) // 16 chars + .onion = 22 chars
    Log::get().error("Invalid length of hostname file!");
  return hostname;
}

/*
https://github.com/Jesse-V/OnioNS-server/blob/a71420f833458b8848745f8334756a825bc55037/src/Mirror.cpp

void Server::start()
{
  // loadState();

  if (!qNode_)
  {
    using namespace std::chrono;
    auto start = steady_clock::now();

    jsonrpc::Socks5Client torTunnel(
        "localhost", std::to_string(socksPort_),
        "http://4fotismd7hyxjvsf.onion:" + std::to_string(Const::SERVER_PORT));
    SpecClient client(torTunnel);
*/
