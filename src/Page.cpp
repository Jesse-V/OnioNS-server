
#include "Page.hpp"
#include <onions-common/Common.hpp>
#include <onions-common/Log.hpp>
#include <onions-common/crypto/ed25519.h>
#include <botan/base64.h>
#include <botan/sha2_64.h>


Page::Page(const Json::Value& value)
{
  Botan::base64_decode(prevHash_.data(), value["prevHash"].asString(), false);
  Botan::base64_decode(rand_.data(), value["rand"].asString(), false);
  Botan::base64_decode(publicEd_.data(), value["publicEd"].asString(), false);
  Botan::base64_decode(pageSig_.data(), value["pageSig"].asString(), false);

  auto records = value["recordList"];
  for (uint j = 0; j < records.size(); j++)
    recordList_.push_back(Common::parseRecord(records[j]));
}



Page::Page(const SHA384_HASH& rand, const ED_KEY& publicEd)
    : rand_(rand), publicEd_(publicEd)
{
  prevHash_.fill(0);
  pageSig_.fill(0);
}



Page::Page(const SHA384_HASH& prevHash,
           const SHA384_HASH& rand,
           const ED_KEY& publicEd)
    : prevHash_(prevHash), rand_(rand), publicEd_(publicEd)
{
  pageSig_.fill(0);
}



bool Page::isValid(bool deepTest)
{  // todo: deep inspection check

  Json::FastWriter writer;
  std::string data = writer.write(getCommonData());
  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data.c_str());
  int check =
      ed25519_sign_open(bytes, data.size(), publicEd_.data(), pageSig_.data());

  if (check == 0)
    return true;
  else if (check == 1)
    return false;
  else
  {
    Log::get().warn("General Ed25519 signature failure on Page signature.");
    return false;
  }
}



void Page::addRecord(const RecordPtr& record)
{
  recordList_.push_back(record);
}



void Page::resign(const std::array<uint8_t, Const::ED25519_KEY_LEN>& sk)
{
  ed25519_public_key pk;
  ed25519_publickey(sk.data(), pk);

  Json::FastWriter writer;
  std::string data = writer.write(getCommonData());
  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data.c_str());
  ed25519_sign(bytes, data.size(), sk.data(), pk, pageSig_.data());
}



Json::Value Page::getCommonData() const
{
  Json::Value obj;
  obj["prevHash"] = Botan::base64_encode(prevHash_.data(), prevHash_.size());
  obj["rand"] = Botan::base64_encode(rand_.data(), rand_.size());

  Json::Value records;
  for (auto r : recordList_)
    records.append(r->asJSONObj());
  obj["recordList"] = records;

  return obj;
}



SHA384_HASH Page::toHash() const
{
  Json::FastWriter writer;
  std::string str = writer.write(getCommonData());
  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(str.c_str());

  SHA384_HASH array;
  Botan::SHA_384 sha384;
  memcpy(array.data(), sha384.process(bytes, str.size()), Const::SHA384_LEN);
  return array;
}



std::string Page::toString() const
{
  Json::Value obj = getCommonData();
  obj["publicEd"] = Botan::base64_encode(publicEd_.data(), publicEd_.size());
  obj["pageSig"] = Botan::base64_encode(pageSig_.data(), pageSig_.size());

  Json::FastWriter writer;
  return writer.write(obj);
}
