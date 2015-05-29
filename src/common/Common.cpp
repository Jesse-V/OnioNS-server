
#include "Common.hpp"
#include "../common/records/CreateR.hpp"
#include "utils.hpp"
#include <botan/sha2_64.h>
#include <fstream>
#include <iostream>


RecordPtr Common::parseRecord(const std::string& json)
{
  return parseRecord(toJSON(json));
}



RecordPtr Common::parseRecord(const Json::Value& rVal)
{
  RecordPtr r = assembleRecord(rVal);
  checkValidity(r);
  return r;
}



Json::Value Common::toJSON(const std::string& json)
{
  Json::Value rVal;
  Json::Reader reader;

  if (!reader.parse(json, rVal))
    throw std::invalid_argument("Failed to parse Record!");

  return rVal;
}



std::string Common::getDestination(const RecordPtr& record,
                                   const std::string& source)
{
  if (record->getName() == source)
    return record->getOnion();

  NameList list = record->getSubdomains();
  for (auto subdomain : list)
    if (subdomain.first == source)
      return subdomain.second;

  throw std::runtime_error("Record does not contain \"" + source + "\"!");
}



uint8_t* Common::computeConsensusHash()
{
  std::cout << "Reading network consensus... ";

  // https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
  std::fstream certsFile("/var/lib/tor-onions/cached-certs");
  std::fstream netStatFile("/var/lib/tor-onions/cached-microdesc-consensus");

  if (!certsFile)
    throw std::runtime_error("Cannot open consensus documents!");

  std::string certsStr((std::istreambuf_iterator<char>(certsFile)),
                       std::istreambuf_iterator<char>());
  std::string netStatStr((std::istreambuf_iterator<char>(netStatFile)),
                         std::istreambuf_iterator<char>());
  std::string consensusStr = certsStr + netStatStr;

  std::cout << "done. (" << consensusStr.length() << " bytes)" << std::endl;

  uint8_t* cHash = new uint8_t[Environment::SHA384_LEN];
  Botan::SHA_384 sha;
  memcpy(cHash, sha.process(consensusStr), Environment::SHA384_LEN);

  return cHash;
}



// ************************** PRIVATE METHODS ****************************** //



RecordPtr Common::assembleRecord(const Json::Value& rVal)
{
  auto cHash = rVal["cHash"].asString();
  auto contact = rVal["contact"].asString();
  auto nonce = rVal["nonce"].asString();
  auto pow = rVal["pow"].asString();
  auto pubHSKey = rVal["pubHSKey"].asString();
  auto sig = rVal["recordSig"].asString();
  auto timestamp = rVal["timestamp"].asString();
  auto type = rVal["type"].asString();
  auto name = rVal["name"].asString();

  if (type != "Create")
    throw std::invalid_argument("Record parsing: not a Create Record!");

  NameList subdomains;
  if (rVal.isMember("subd"))
  {
    Json::Value list = rVal["subd"];
    auto sources = list.getMemberNames();
    for (auto source : sources)
      subdomains.push_back(std::make_pair(source, list[source].asString()));
  }

  auto key = Utils::base64ToRSA(pubHSKey);
  return std::make_shared<CreateR>(cHash, contact, name, subdomains, nonce, pow,
                                   sig, key, timestamp);
}



void Common::checkValidity(const RecordPtr& r)
{
  std::cout << "Checking validity... ";
  std::cout.flush();
  bool tmp = false;
  r->computeValidity(&tmp);
  std::cout << "done." << std::endl;

  if (r->hasValidSignature())
    std::cout << "Record signature is valid." << std::endl;
  else
    throw std::runtime_error("Bad signature on Record!");

  if (r->isValid())  // todo: this does not actually check the PoW output
    std::cout << "Record proof-of-work is valid." << std::endl;
  else
    throw std::runtime_error("Record is not valid!");

  std::cout << "Record check complete." << std::endl;
}
