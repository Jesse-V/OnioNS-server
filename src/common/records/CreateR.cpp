
#include "CreateR.hpp"
#include "../utils.hpp"
#include <botan/base64.h>
#include <json/json.h>


CreateR::CreateR(Botan::RSA_PrivateKey* key, uint8_t* consensusHash,
   const std::string& name, const std::string& contact):
   Record(key, consensusHash)
{
   setName(name);
   setContact(contact);
}



bool CreateR::setName(const std::string& newName)
{
   if (newName.empty() || newName.length() > 32)
      return false;

   name_ = newName;
   valid_ = false;
   return true;
}



bool CreateR::addSubdomain(const std::string& from, const std::string& to)
{
   if (subdomains_.size() >= 16 || from.size() > 32 || to.size() > 32)
      return false;

   subdomains_.push_back(std::make_pair(from, to));
   valid_ = false; //need new nonce now

   return true;
}



bool CreateR::setContact(const std::string& contactInfo)
{
   if (!Utils::isPowerOfTwo(contactInfo.length()))
      return false;

   contact_ = contactInfo;
   valid_ = false; //need new nonce now
   return true;
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
   Json::Value nameList;
   nameList[name_] = getOnion();
   for (auto sub : subdomains_)
      nameList[sub.first] = sub.second;
   obj["nameList"] = nameList;

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
   os << "   Name: " << dt.name_ << " -> " << dt.getOnion() << std::endl;
   os << "   Subdomains: ";

   if (dt.subdomains_.empty())
      os << "(none)";
   else
      for (auto subd : dt.subdomains_)
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
   str += name_;
   for (auto subd : subdomains_)
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
