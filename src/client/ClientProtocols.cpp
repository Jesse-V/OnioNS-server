
#include "ClientProtocols.hpp"
#include "tcp/IPC.hpp"
#include "../common/records/CreateR.hpp"
#include "../common/utils.hpp"
#include <botan/base64.h>
#include <iostream>


void ClientProtocols::listenForDomains()
{
  // establish connection with remote resolver over Tor
  if (!connectToResolver())
    return;

  IPC ipc(9053);
  ipc.start();
}



std::string ClientProtocols::resolve(const std::string& torDomain)
{
  try
  {
    std::string domain = torDomain;

    while (Utils::strEndsWith(domain, ".tor"))
    {
      // check cache first
      auto iterator = cache_.find(domain);
      if (iterator == cache_.end())
      {
        std::cout << "Sending \"" << domain << "\" to resolver..." << std::endl;
        auto response = remoteResolver_->sendReceive(domain + "\r\n");
        std::cout << "Received Record response." << std::endl;

        auto dest = getDestination(parseRecord(response), domain);

        cache_[domain] = dest;
        domain = dest;
      }
      else
        domain = iterator->second;  // retrieve from cache
    }

    if (domain.length() != 22 || !Utils::strEndsWith(domain, ".onion"))
      throw std::runtime_error("\"" + domain + "\" is not a HS address!");
    return domain;
  }
  catch (std::runtime_error& re)
  {
    std::cerr << "Err: " << re.what() << std::endl;
  }

  return "xxxxxxxxxxxxxxxx.onion";
}



std::shared_ptr<Record> ClientProtocols::parseRecord(const std::string& json)
{
  Json::Value rVal = toJSON(json);

  auto cHash = rVal["cHash"].asString();
  auto contact = rVal["contact"].asString();
  auto nonce = rVal["nonce"].asString();
  auto pow = rVal["pow"].asString();
  auto pubHSKey = rVal["pubHSKey"].asString();
  auto recordSig = rVal["recordSig"].asString();
  auto timestamp = rVal["timestamp"].asString();
  auto type = rVal["type"].asString();

  if (type != "Create")
    throw std::invalid_argument("Record parsing: not a Create Record!");

  NameList nameList;
  Json::Value list = rVal["nameList"];
  auto sources = list.getMemberNames();
  for (auto source : sources)
    nameList.push_back(std::make_pair(source, list[source].asString()));

  // std::cout << "done." << std::endl;

  // std::cout << "Decoding into Record... ";
  auto key = base64ToRSA(pubHSKey);
  auto createR = std::make_shared<CreateR>(cHash, contact, nameList, nonce, pow,
                                           recordSig, key, timestamp);
  // std::cout << "done." << std::endl;

  std::cout << "Checking validity... ";
  std::cout.flush();
  bool tmp = false;
  createR->computeValidity(&tmp);
  std::cout << "done." << std::endl;

  if (createR->hasValidSignature())
    std::cout << "Record signature is valid." << std::endl;
  else
    throw std::runtime_error("Bad signature on Record!");

  if (createR->isValid())
    std::cout << "Record proof-of-work is valid." << std::endl;
  else
    throw std::runtime_error("Record is not valid!");

  std::cout << "Record check complete." << std::endl;

  return createR;
}



Json::Value ClientProtocols::toJSON(const std::string& json)
{
  // std::cout << "Parsing JSON... ";

  Json::Value rVal;
  Json::Reader reader;

  if (!reader.parse(json, rVal))
    throw std::invalid_argument("Failed to parse Record!");

  if (!rVal.isMember("nameList"))
    throw std::invalid_argument("Record parsing: missing NameList");

  return rVal;
}



std::string ClientProtocols::getDestination(
    const std::shared_ptr<Record>& record,
    const std::string& source)
{
  NameList list = record->getNameList();
  for (auto pair : list)
    if (pair.first == source)
      return pair.second;

  throw std::runtime_error("Record does not contain \"" + source + "\"!");
}



// ***************************** PRIVATE METHODS *****************************



Botan::RSA_PublicKey* ClientProtocols::base64ToRSA(const std::string& base64)
{
  // decode public key
  unsigned long expectedSize = Utils::decode64Estimation(base64.length());
  uint8_t* keyBuffer = new uint8_t[expectedSize];
  size_t len = Botan::base64_decode(keyBuffer, base64, false);

  // interpret and parse into public RSA key
  std::istringstream iss(std::string(reinterpret_cast<char*>(keyBuffer), len));
  Botan::DataSource_Stream keyStream(iss);
  return dynamic_cast<Botan::RSA_PublicKey*>(Botan::X509::load_key(keyStream));
}



bool ClientProtocols::connectToResolver()
{
  try
  {
    std::cout << "Starting client functionality..." << std::endl;

    // connect over Tor to remote resolver
    remoteResolver_ = std::make_shared<SocksClient>("localhost", 9050);
    remoteResolver_->connectTo("129.123.7.8", 10053);
  }
  catch (boost::system::system_error const& ex)
  {
    std::cerr << ex.what() << std::endl;
    std::cerr << "Tor does not appear to be running! Aborting." << std::endl;
    return false;
  }
  catch (std::exception& ex)
  {
    std::cerr << ex.what() << std::endl;
    return false;
  }

  return true;
}
