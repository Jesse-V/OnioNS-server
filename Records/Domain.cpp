
#include "Domain.hpp"
#include "../utils.hpp"
#include <botan-1.10/botan/sha2_32.h>
#include <botan/base64.h>
#include <thread>
#include <cstring>
#include <cassert>
#include <iostream>

/*
    let "central" be:
        std::string name_;
        std::vector<std::pair<std::string,std::string>> subdomains_;
        uint8_t consensusHash_[SHA256_LEN];
        uint8_t nonce_[NONCE_LEN];
        std::string contact_;
        long timestamp_;
        pubKey

    for each nonce, generate:
        uint8_t scrypted_[SCRYPTED_LEN]; //scrypt output of {central}
        uint8_t signature_[SIGNATURE_LEN]; //digital sig of {central, scrypted_}
    registration valid iff SHA384{central, scrypted_, signature_} < THRESHOLD
    then save as {central, scrypted_, signature_} in JSON format

    computational operator won't know when to stop, since the sig matters
    must use deterministic sig alg!
*/

Domain::Domain(const std::string& name, uint8_t consensusHash[SHA256_LEN],
    const std::string& contact, Botan::RSA_PrivateKey* key):
    timestamp_(time(NULL)), valid_(false)
{
    assert(key->get_n().bits() == RSA_LEN);

    setName(name);
    setContact(contact);
    setKey(key);

    memcpy(consensusHash_, consensusHash, SHA256_LEN);
    memset(nonce_, 0, NONCE_LEN);
    memset(scrypted_, 0, SCRYPTED_LEN);
    memset(signature_, 0, SIGNATURE_LEN);
}



Domain::~Domain()
{
    //delete signature_;
    //delete nonce_;
}



bool Domain::setName(const std::string& newName)
{
    if (newName.empty() || newName.length() > 32)
        return false;

    name_ = newName;
    valid_ = false;
    return true;
}



bool Domain::addSubdomain(const std::string& from, const std::string& to)
{
    if (subdomains_.size() >= 16 || from.size() > 32 || to.size() > 32)
        return false;

    subdomains_.push_back(std::make_pair(from, to));
    valid_ = false; //need new nonce now

    return true;
}



bool Domain::setContact(const std::string& contactInfo)
{
    if (!Utils::isPowerOfTwo(contactInfo.length()))
        return false;

    contact_ = contactInfo;
    valid_ = false; //need new nonce now
    return true;
}



bool Domain::setKey(Botan::RSA_PrivateKey* key)
{
    if (key == NULL)
        return false;

    key_ = key;
    valid_ = false; //need new nonce now
    return true;
}



bool Domain::refresh()
{
    timestamp_ = time(NULL);
    //consensusHash_ = //TODO

    valid_ = false; //need new nonce now
    return true;
}



bool Domain::makeValid()
{
    //TODO: if issue with fields other than nonce, return false

    return mineParallel(1);
}



bool Domain::isValid() const
{
    return valid_;
}



std::string Domain::getOnion() const
{
    return "temp.onion"; //TODO: calculate hash
}



UInt32Data Domain::getPublicKey() const
{
    //https://en.wikipedia.org/wiki/X.690#BER_encoding
    auto bem = Botan::X509::BER_encode(*key_);
    uint8_t* val = new uint8_t[bem.size()];
    memcpy(val, bem, bem.size());

    return std::make_pair(val, bem.size());
}



std::string Domain::asJSON() const
{
    std::string str;
    str += "{\"name\":\"" + name_;

    str += "\",\"subd\":{";
    for (auto sub : subdomains_)
        str += "\"" + sub.first + "\":\"" + sub.second + "\",";
    if (!subdomains_.empty())
        str.pop_back(); //remove trailing comma

    str += "},\"pgp\":\"" + contact_;
    str += "\",\"t\":" + std::to_string(timestamp_);
    str += ",\"cHash\":\"" + Botan::base64_encode(consensusHash_, SHA256_LEN);

    str += "\",\"n\":\"";
    if (isValid())
        str += Botan::base64_encode(nonce_, NONCE_LEN);

    str += "\",\"scrypt\":\"";
    if (isValid())
        str += Botan::base64_encode(scrypted_, SCRYPTED_LEN);

    str += "\",\"sig\":\"";
    if (isValid())
        str += Botan::base64_encode(signature_, SIGNATURE_LEN);

    auto ber = Botan::X509::BER_encode(*key_);
    uint8_t* berBin = new uint8_t[ber.size()];
    memcpy(berBin, ber, ber.size());

    str += "\",\"pubKey\":\"" + Botan::base64_encode(berBin, ber.size()) + "\"";

    return str + "}";
}



std::ostream& operator<<(std::ostream& os, const Domain& dt)
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

    os << "      Day Consensus: " <<
        Botan::base64_encode(dt.consensusHash_, dt.SHA256_LEN) << std::endl;

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


//********************* PRIVATE METHODS ****************************************


UInt32Data Domain::getCentral(uint8_t* nonce) const
{
    std::string str;
    str += name_;
    for (auto subd : subdomains_)
        str += subd.first + subd.second;
    str += contact_;
    str += std::to_string(timestamp_);

    int index = 0;
    auto pubKey = getPublicKey();
    const size_t centralLen = str.length() + SHA256_LEN + NONCE_LEN + pubKey.second;
    uint8_t* central = new uint8_t[centralLen];

    memcpy(central + index, str.c_str(), str.size()); //copy string into array
    index += str.size();

    memcpy(central + index, consensusHash_, SHA256_LEN);
    index += SHA256_LEN;

    memcpy(central + index, nonce, NONCE_LEN);
    index += NONCE_LEN;

    memcpy(central + index, pubKey.first, pubKey.second);

    //std::cout << Botan::base64_encode(central, centralLen) << std::endl;

    return std::make_pair(central, centralLen);
}



bool Domain::mineParallel(uint nInstances)
{
    if (nInstances == 0)
        return WorkStatus::Aborted;

    auto nonces = new uint8_t[nInstances][NONCE_LEN];
    auto scryptOuts = new uint8_t[nInstances][SCRYPTED_LEN];
    auto sigs = new uint8_t[nInstances][SIGNATURE_LEN];

    //Domain::WorkStatus status = WorkStatus::Success;
    std::vector<std::thread> workers;
    for (uint n = 0; n < nInstances; n++)
    {
        workers.push_back(std::thread([n, nInstances, nonces, scryptOuts, sigs]()
        {
            std::cout << nInstances << std::endl;
            std::cout << "Spawning worker " << n << " / " << nInstances << std::endl;

            //prepare dynamic variables for this instance
            memset(nonces[n], 0, NONCE_LEN);
            memset(scryptOuts[n], 0, SCRYPTED_LEN);
            memset(sigs[n], 0, SIGNATURE_LEN);
            nonces[n][NONCE_LEN - 1] = n;

            auto ret = makeValid(0, nInstances, nonces[n], scryptOuts[n], sigs[n]);
            if (ret == WorkStatus::Success)
            {
                memcpy(nonce_, nonces[n], NONCE_LEN);
                memcpy(scrypted_, scryptOuts[n], SCRYPTED_LEN);
                memcpy(signature_, sigs[n], SIGNATURE_LEN);
                //return WorkStatus::Success;
            }
        }));
    }

    std::for_each(workers.begin(), workers.end(), [](std::thread &t)
    {
        t.join();
    });

    //static: getCentral(), pubKey
    //dynamic: nonce, scrypted_, signature_

    return true; //temp
}



Domain::WorkStatus Domain::makeValid(uint8_t depth, uint8_t inc,
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

        const auto sigInLen = central.second + SCRYPTED_LEN;
        const auto totalLen = sigInLen + SIGNATURE_LEN;

        //save {central, scryptedBuf} with room for signature
        uint8_t* buffer = new uint8_t[totalLen];
        memcpy(buffer, central.first, central.second); //import central
        memcpy(buffer + central.second, scryptedBuf, SCRYPTED_LEN); //import scryptedBuf

        //digitally sign (RSA-SHA512) {central, scryptedBuf}
        signMessageDigest(buffer, sigInLen, key_, sigBuf);
        memcpy(buffer + sigInLen, sigBuf, SIGNATURE_LEN);

        //hash (SHA-256) {central, scryptedBuf, sigBuf}
        Botan::SHA_256 sha256;
        auto hash = sha256.process(buffer, totalLen);

        //interpret hash output as number and compare against threshold
        auto num = Utils::arrayToUInt32(hash, 0);
        std::cout << Botan::base64_encode(nonceBuf, NONCE_LEN) << " -> " << num << std::endl;

        if (isValid())
            return WorkStatus::Aborted;

        if (num < THRESHOLD)
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
