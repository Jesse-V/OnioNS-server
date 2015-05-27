
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
  setName(primaryName);
  setContact(contact);
}



CreateR::CreateR(const std::string& cHash,
                 const std::string& contact,
                 const std::string& name,
                 const NameList& subdomains,
                 const std::string& nonce,
                 const std::string& pow,
                 const std::string& sig,
                 Botan::RSA_PublicKey* pubKey,
                 const std::string& time)
    : Record(pubKey)
{
  type_ = "Create";
  setContact(contact);
  setName(name);
  setSubdomains(subdomains);
  timestamp_ = stol(time);

  assert(Botan::base64_decode(consensusHash_, cHash, false) ==
         Environment::SHA384_LEN);
  assert(Botan::base64_decode(nonce_, nonce, false) == NONCE_LEN);
  assert(Botan::base64_decode(scrypted_, pow, false) == SCRYPTED_LEN);
  assert(Botan::base64_decode(signature_, sig, false) ==
         Environment::SIGNATURE_LEN);
}
