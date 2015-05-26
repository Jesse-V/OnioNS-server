
#include "CreateR.hpp"
#include "../../common/Common.hpp"
#include <botan/base64.h>
#include <cassert>
#include <iostream>


CreateR::CreateR(Botan::RSA_PrivateKey* key,
                 const std::string& primaryName,
                 const std::string& contact)
    : Record(key, Common::get().computeConsensusHash())
{
  type_ = "Create";

  NameList nameList;
  nameList.push_back(std::make_pair(primaryName, getOnion()));
  setNameList(nameList);

  setContact(contact);
}



CreateR::CreateR(const std::string& cHash,
                 const std::string& contact,
                 const NameList& nameList,
                 const std::string& nonce,
                 const std::string& pow,
                 const std::string& sig,
                 Botan::RSA_PublicKey* pubKey,
                 const std::string& time)
    : Record(pubKey)
{
  type_ = "Create";
  contact_ = contact;
  nameList_ = nameList;
  timestamp_ = stol(time);

  assert(Botan::base64_decode(consensusHash_, cHash, false) ==
         Environment::SHA384_LEN);
  assert(Botan::base64_decode(nonce_, nonce, false) == NONCE_LEN);
  assert(Botan::base64_decode(scrypted_, pow, false) == SCRYPTED_LEN);
  assert(Botan::base64_decode(signature_, sig, false) ==
         Environment::SIGNATURE_LEN);
}
