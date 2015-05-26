
#include "Common.hpp"
#include "../common/records/CreateR.hpp"
#include "utils.hpp"
#include <botan/sha2_64.h>
#include <fstream>
#include <iostream>


std::shared_ptr<Record> Common::parseRecord(const std::string& json)
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
  auto key = Utils::base64ToRSA(pubHSKey);
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



Json::Value Common::toJSON(const std::string& json)
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



std::string Common::getDestination(const std::shared_ptr<Record>& record,
                                   const std::string& source)
{
  NameList list = record->getNameList();
  for (auto pair : list)
    if (pair.first == source)
      return pair.second;

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
