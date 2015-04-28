
#include "Record.hpp"
#include "../utils.hpp"

#include <libscrypt/libscrypt.h>
#include <botan/pubkey.h>
#include <botan/sha160.h>
#include <botan/sha2_32.h>
#include <botan/base64.h>
#include <CyoEncode/CyoEncode.hpp>

#include <thread>
#include <cassert>
#include <iostream>


Record::Record(Botan::RSA_PrivateKey* key, uint8_t* consensusHash):
   timestamp_(time(NULL)), valid_(false)
{
   assert(key->get_n().bits() == RSA_LEN);
   setKey(key);

   memcpy(consensusHash_, consensusHash, SHA384_LEN);
   memset(nonce_, 0, NONCE_LEN);
   memset(scrypted_, 0, SCRYPTED_LEN);
   memset(signature_, 0, SIGNATURE_LEN);
}



Record::~Record()
{
   //delete signature_;
   //delete nonce_;
}



bool Record::setKey(Botan::RSA_PrivateKey* key)
{
   if (key == NULL)
      return false;

   key_ = key;
   valid_ = false; //need new nonce now
   return true;
}



UInt32Data Record::getPublicKey() const
{
   //https://en.wikipedia.org/wiki/X.690#BER_encoding
   auto bem = Botan::X509::BER_encode(*key_);
   uint8_t* val = new uint8_t[bem.size()];
   memcpy(val, bem, bem.size());

   return std::make_pair(val, bem.size());
}



std::string Record::getOnion() const
{
   //https://gitweb.torproject.org/torspec.git/tree/tor-spec.txt :
      // When we refer to "the hash of a public key", we mean the SHA-1 hash of the DER encoding of an ASN.1 RSA public key (as specified in PKCS.1).

   //get DER encoding of RSA key
   auto x509Key = key_->x509_subject_public_key();
   char* derEncoded = new char[x509Key.size()];
   memcpy(derEncoded, x509Key, x509Key.size());

   //perform SHA-1
   Botan::SHA_160 sha1;
   auto hash = sha1.process(std::string(derEncoded, x509Key.size()));
   delete derEncoded;

   //perform base32 encoding
   char onionB32[SHA1_LEN * 4];
   CyoEncode::Base32::Encode(onionB32, hash, SHA1_LEN);

   //truncate, make lowercase, and return result
   auto addr = std::string(onionB32, 16);
   std::transform(addr.begin(), addr.end(), addr.begin(), ::tolower);
   return addr + ".onion";
}



bool Record::refresh()
{
   timestamp_ = time(NULL);
   //consensusHash_ = //TODO

   valid_ = false; //need new nonce now
   return true;
}



bool Record::isValid() const
{
   return valid_;
}



// ***************************** PRIVATE METHODS *****************************



Record::WorkStatus Record::mineParallel(uint8_t nInstances)
{
   if (nInstances == 0)
      return WorkStatus::Aborted;

   auto nonces = new uint8_t[nInstances][NONCE_LEN];
   auto scryptOuts = new uint8_t[nInstances][SCRYPTED_LEN];
   auto sigs = new uint8_t[nInstances][SIGNATURE_LEN];

   //Record::WorkStatus status = WorkStatus::Success;
   std::vector<std::thread> workers;
   for (uint8_t n = 0; n < nInstances; n++)
   {
      workers.push_back(std::thread([n, nInstances, nonces, scryptOuts, sigs, this]()
      {
         std::string name("worker ");
         name += std::to_string(n+1) + "/" + std::to_string(nInstances);

         std::cout << "Starting " << name << std::endl;

         //prepare dynamic variables for this instance
         memset(nonces[n], 0, NONCE_LEN);
         memset(scryptOuts[n], 0, SCRYPTED_LEN);
         memset(sigs[n], 0, SIGNATURE_LEN);
         nonces[n][NONCE_LEN - 1] = n;

         auto ret = makeValid(0, nInstances, nonces[n], scryptOuts[n], sigs[n]);
         if (ret == WorkStatus::Success)
         {
            std::cout << "Success from " << name << std::endl;

            //save successful answer
            memcpy(nonce_, nonces[n], NONCE_LEN);
            memcpy(scrypted_, scryptOuts[n], SCRYPTED_LEN);
            memcpy(signature_, sigs[n], SIGNATURE_LEN);
         }

         std::cout << "Shutting down " << name << std::endl;
      }));
   }

   std::for_each(workers.begin(), workers.end(), [](std::thread &t)
   {
      t.join();
   });

   return WorkStatus::Success;
}



Record::WorkStatus Record::makeValid(uint8_t depth, uint8_t inc,
   uint8_t* nonceBuf, uint8_t* scryptedBuf, uint8_t* sigBuf)
{
   if (isValid())
      return WorkStatus::Aborted;

   if (depth > NONCE_LEN)
      return WorkStatus::NotFound;

   if (depth == NONCE_LEN)
   {
      //run central domain info through scrypt, save output to scryptedBuf
      auto central = getCentral(nonceBuf);
      if (scrypt(central.first, central.second, scryptedBuf) < 0)
      {
         std::cout << "Error with scrypt call!" << std::endl;
         return WorkStatus::Aborted;
      }

      if (isValid())
         return WorkStatus::Aborted;

      const auto sigInLen = central.second + SCRYPTED_LEN;
      const auto totalLen = sigInLen + SIGNATURE_LEN;

      //save {central, scryptedBuf} with room for signature
      uint8_t* buffer = new uint8_t[totalLen];
      memcpy(buffer, central.first, central.second); //import central
      memcpy(buffer + central.second, scryptedBuf, SCRYPTED_LEN); //import scryptedBuf

      //digitally sign (RSA-SHA384) {central, scryptedBuf}
      signMessageDigest(buffer, sigInLen, key_, sigBuf);
      memcpy(buffer + sigInLen, sigBuf, SIGNATURE_LEN);

      //hash (SHA-256) {central, scryptedBuf, sigBuf}
      Botan::SHA_256 sha256;
      auto hash = sha256.process(buffer, totalLen);

      //interpret hash output as number and compare against threshold
      auto num = Utils::arrayToUInt32(hash, 0);
      std::cout << Botan::base64_encode(nonceBuf, NONCE_LEN) << " -> " << num << std::endl;
      std::cout.flush();

      if (isValid())
         return WorkStatus::Aborted;

      if (num < UINT32_MAX / (1 << getDifficulty()))
      {
         valid_ = true;
         return WorkStatus::Success;
      }

      return WorkStatus::NotFound;
   }

   WorkStatus ret = makeValid(depth + 1, inc, nonceBuf, scryptedBuf, sigBuf);
   if (ret == WorkStatus::Success || ret == WorkStatus::Aborted)
      return ret;

   while (nonceBuf[depth] < UINT8_MAX)
   {
      nonceBuf[depth] += inc;
      ret = makeValid(depth + 1, inc, nonceBuf, scryptedBuf, sigBuf);
      if (ret == WorkStatus::Success || ret == WorkStatus::Aborted)
         return ret;
   }

   nonceBuf[depth] = 0;
   return WorkStatus::NotFound;
}



size_t Record::signMessageDigest(const uint8_t* message, size_t length,
   const Botan::Private_Key* key, uint8_t* sigBuf) const
{
   static Botan::AutoSeeded_RNG rng;

   //https://stackoverflow.com/questions/14263346/how-to-perform-asymmetric-encryption-with-botan
   //http://botan.randombit.net/manual/pubkey.html#signatures
   Botan::PK_Signer signer(*key, "EMSA-PKCS1-v1_5(SHA-384)");
   auto sig = signer.sign_message(message, length, rng);

   assert(sig.size() == SIGNATURE_LEN);
   memcpy(sigBuf, sig, sig.size());

   return sig.size();
}



int Record::scrypt(const uint8_t* input, size_t inputLen, uint8_t* output) const
{
   //allocate and prepare static salt
   static uint8_t* const SALT = new uint8_t[SCRYPT_SALT_LEN];
   static bool saltReady = false;
   if (!saltReady)
   {
      assert(SCRYPT_SALT_LEN == 16);
      std::string piHex("243F6A8885A308D313198A2E03707344"); //pi in hex
      Utils::hex2bin(piHex.c_str(), SALT);
      saltReady = true;
   }

   return libscrypt_scrypt(input, inputLen, SALT, SCRYPT_SALT_LEN,
      SCR_N, 1, SCR_P, output, SCRYPTED_LEN);
}
