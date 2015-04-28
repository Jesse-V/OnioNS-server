
#include "CreateR.hpp"
#include "../../common/CommonProtocols.hpp"
#include "../utils.hpp"
#include <botan/base64.h>
#include <json/json.h>


CreateR::CreateR(Botan::RSA_PrivateKey* key,
   const std::string& primaryName, const std::string& contact):
   Record(key, CommonProtocols::get().computeConsensusHash())
{
   NameList nameList;
   nameList.push_back(std::make_pair(primaryName, getOnion()));
   setNameList(nameList);

   setContact(contact);
}



void CreateR::setNameList(const NameList& nameList)
{
   //todo: count/check number of second-level domain names
   //todo: count/check number and length of names

   if (nameList.empty() || nameList.size() > 24)
      throw std::invalid_argument("Name list of invalid length!");

   for (auto pair : nameList)
   {
      if (pair.first.length() <= 5 || pair.first.length() > 128)
         throw std::invalid_argument("Invalid length of source name!");
      if (pair.second.length() <= 5 || pair.second.length() > 128)
         throw std::invalid_argument("Invalid length of destination name!");

      if (!Utils::strEndsWith(pair.first, ".tor"))
         throw std::invalid_argument("Source name must begin with .tor!");
      if (!Utils::strEndsWith(pair.first, ".tor") &&
         !Utils::strEndsWith(pair.first, ".onion"))
         throw std::invalid_argument("Destination must go to .tor or .onion!");
   }

   nameList_ = nameList;
   valid_ = false;
}



NameList CreateR::getNameList()
{
   return nameList_;
}



void CreateR::setContact(const std::string& contactInfo)
{
   if (!Utils::isPowerOfTwo(contactInfo.length()))
      throw std::invalid_argument("Invalid length of PGP key");

   contact_ = contactInfo;
   valid_ = false;
}



bool CreateR::makeValid(uint8_t nCPUs)
{
   //TODO: if issue with fields other than nonce, return false

   return mineParallel(nCPUs);
}



uint32_t CreateR::getDifficulty() const
{
   return 6; // 1/2^x chance of success, so order of magnitude
}



std::string CreateR::asJSON() const
{
   Json::Value obj;

   obj["type"] = "Create";
   obj["contact"] = contact_;
   obj["timestamp"] = std::to_string(timestamp_);
   obj["cHash"] = Botan::base64_encode(consensusHash_, SHA384_LEN);

   //add names and subdomains
   for (auto sub : nameList_)
      obj["nameList"][sub.first] = sub.second;

   //extract and save public key
   auto ber = Botan::X509::BER_encode(*key_);
   uint8_t* berBin = new uint8_t[ber.size()];
   memcpy(berBin, ber, ber.size());
   obj["pubHSKey"] = Botan::base64_encode(berBin, ber.size());

   //if the domain is valid, add nonce_, scrypted_, and signature_
   if (isValid())
   {
      obj["nonce"] = Botan::base64_encode(nonce_, NONCE_LEN);
      obj["pow"] = Botan::base64_encode(scrypted_, SCRYPTED_LEN);
      obj["recordSig"] = Botan::base64_encode(signature_, SIGNATURE_LEN);
   }

   //output in compressed (non-human-friendly) format
   Json::FastWriter writer;
   return writer.write(obj);
}



std::ostream& operator<<(std::ostream& os, const CreateR& dt)
{
   os << "Domain Registration: (currently " <<
      (dt.valid_ ? "VALID)" : "INVALID)") << std::endl;
   os << "   Name: DERP -> " << dt.getOnion() << std::endl;
   os << "   Subdomains: ";

   for (auto subd : dt.nameList_)
      os << std::endl << "      " << subd.first << " -> " << subd.second;
   os << std::endl;

   os << "   Contact: 0x" << dt.contact_ << std::endl;
   os << "   Time: " << dt.timestamp_ << std::endl;
   os << "   Validation:" << std::endl;

   os << "      Last Consensus: " <<
      Botan::base64_encode(dt.consensusHash_, dt.SHA384_LEN) << std::endl;

   os << "      Nonce: ";
   if (dt.isValid())
      os << Botan::base64_encode(dt.nonce_, dt.NONCE_LEN) << std::endl;
   else
      os << "<regeneration required>" << std::endl;

   os << "      Proof of Work: ";
   if (dt.isValid())
      os << Botan::base64_encode(dt.scrypted_, dt.SCRYPTED_LEN) << std::endl;
   else
      os << "<regeneration required>" << std::endl;

   os << "      Signature: ";
   if (dt.isValid())
      os << Botan::base64_encode(dt.signature_, dt.SIGNATURE_LEN / 4) <<
         " ..." << std::endl;
   else
      os << "<regeneration required>" << std::endl;

   auto pem = Botan::X509::PEM_encode(*dt.key_);
   pem.pop_back(); //delete trailing /n
   Utils::stringReplace(pem, "\n", "\n\t");
   os << "      RSA Public Key: \n\t" << pem;

   return os;
}



// ***************************** PRIVATE METHODS *****************************



UInt32Data CreateR::getCentral(uint8_t* nonce) const
{
   std::string str;
   for (auto subd : nameList_)
      str += subd.first + subd.second;
   str += contact_;
   str += std::to_string(timestamp_);

   int index = 0;
   auto pubKey = getPublicKey();
   const size_t centralLen = str.length() + SHA384_LEN + NONCE_LEN + pubKey.second;
   uint8_t* central = new uint8_t[centralLen];

   memcpy(central + index, str.c_str(), str.size()); //copy string into array
   index += str.size();

   memcpy(central + index, consensusHash_, SHA384_LEN);
   index += SHA384_LEN;

   memcpy(central + index, nonce, NONCE_LEN);
   index += NONCE_LEN;

   memcpy(central + index, pubKey.first, pubKey.second);

   //std::cout << Botan::base64_encode(central, centralLen) << std::endl;

   return std::make_pair(central, centralLen);
}
