
#include "Page.hpp"
#include <onions-common/crypto/ed25519.h>
#include <botan/base64.h>
#include <botan/sha2_64.h>
#include <sstream>


Page::Page(const uint8_t* latestRandom, const uint8_t* routerFingerprint)
    : rand_(latestRandom), fingerprint_(routerFingerprint)
{
  memset(&prevHash_, 0, Const::SHA384_LEN);
  memset(pageSig_, 0, Const::SHA384_LEN);
}



Page::Page(const uint8_t* previousHash,
           const uint8_t* latestRandom,
           const uint8_t* routerFingerprint)
    : prevHash_(previousHash),
      rand_(latestRandom),
      fingerprint_(routerFingerprint)
{
  memset(pageSig_, 0, Const::SHA384_LEN);
}



void Page::addRecord(const RecordPtr& record)
{
  recordList_.push_back(record);
}



void Page::updateSignature(const uint8_t* secretKey)
{
  ed25519_public_key publicKey;
  ed25519_publickey(secretKey, publicKey);

  Json::FastWriter writer;
  std::string data = writer.write(getCommonData());
  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data.c_str());
  ed25519_sign(bytes, data.size(), secretKey, publicKey, pageSig_);
}



Json::Value Page::getCommonData() const
{
  Json::Value obj;
  obj["prevHash"] = Botan::base64_encode(prevHash_, Const::SHA384_LEN);
  obj["rand"] = Botan::base64_encode(rand_, Const::SHA384_LEN);

  Json::Value records;
  for (auto r : recordList_)
    records.append(r->asJSONObj());
  obj["recordList"] = records;

  return obj;
}



uint8_t* Page::toHash() const
{
  Json::FastWriter writer;
  std::string data = writer.write(getCommonData());
  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data.c_str());

  Botan::SHA_384 sha384;
  return sha384.process(bytes, data.size());
}



std::string Page::toString() const
{
  Json::Value obj = getCommonData();
  obj["fingerprint"] = Botan::base64_encode(fingerprint_, Const::SHA384_LEN);
  obj["pageSig"] = Botan::base64_encode(pageSig_, Const::SHA384_LEN);

  Json::FastWriter writer;
  return writer.write(obj);
}
