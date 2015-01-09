
#include "Domain.hpp"
#include "../utils.hpp"
#include <botan-1.10/botan/sha2_32.h>
#include <botan/base64.h>
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

    memset(nonce_, 0, NONCE_LEN);
    memset(signature_, 0, SIGNATURE_LEN);
    return findNonce(0);
}



bool Domain::isValid() const
{
    return valid_;
}



std::string Domain::getOnion() const
{
    return "temp.onion"; //TODO: calculate hash
}



std::pair<uint8_t*, size_t> Domain::getPublicKey() const
{
    //https://en.wikipedia.org/wiki/X.690#BER_encoding
    auto bem = Botan::X509::BER_encode(*key_);
    uint8_t* val = new uint8_t[bem.size()];
    memcpy(val, bem, bem.size());

    return std::make_pair(val, bem.size());
}



std::pair<uint8_t*, size_t> Domain::asJSON() const
{
    std::string str;
    str += "{\"name\":\"" + name_;

    str += "\",\"subd\":{";
    for (auto sub : subdomains_)
        str += "\"" + sub.first + "\":\"" + sub.second + "\",";
    if (!subdomains_.empty())
        str.pop_back(); //remove trailing comma

    str += "},\"cHash\":\"" + Botan::base64_encode(consensusHash_, SHA256_LEN);
    str += "\",\"pgp\":\"" + contact_;

    str += "\",\"sig\":\"";
    if (isValid())
        str += Botan::base64_encode(signature_, SIGNATURE_LEN);

    str += "\",\"n\":\"" + Botan::base64_encode(nonce_, NONCE_LEN);
    str += "\",\"t\":" + std::to_string(timestamp_);
    str += "}";

    //TODO: include pubkey

    //std::cout << str << std::endl;

    return std::make_pair((unsigned char*)str.c_str(), str.size());
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

    //std::string header("-----BEGIN PUBLIC KEY-----");
    auto pem = Botan::X509::PEM_encode(*dt.key_);
    Utils::stringReplace(pem, "\n", "\n\t");
    os << "      RSA Public Key: \n\t" << pem << std::endl;

    return os;
}


//********************* PRIVATE METHODS ****************************************


std::pair<uint8_t*, size_t> Domain::getCentral()
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

    memcpy(central + index, nonce_, NONCE_LEN);
    index += NONCE_LEN;

    memcpy(central + index, pubKey.first, pubKey.second);

    //std::cout << Botan::base64_encode(central, centralLen) << std::endl;

    return std::make_pair(central, centralLen);
}



bool Domain::findNonce(uint8_t depth)
{
    if (depth > NONCE_LEN)
        return false;

    if (depth == NONCE_LEN)
    {
        //run central domain info through scrypt, save output to scrypted_
        auto central = getCentral();
        if (scrypt(central.first, central.second, scrypted_) < 0)
        {
            std::cout << "Error with scrypt call!" << std::endl;
            return false;
        }

        const auto sigInLen = central.second + SCRYPTED_LEN;
        const auto totalLen = sigInLen + SIGNATURE_LEN;

        //save {central, scrypted_} with room for signature
        uint8_t* buffer = new uint8_t[totalLen];
        memcpy(buffer, central.first, central.second); //import central
        memcpy(buffer + central.second, scrypted_, SCRYPTED_LEN); //import scrypted_

        //digitally sign (RSA-SHA512) {central, scrypted_}
        signMessageDigest(buffer, sigInLen, key_, signature_);
        memcpy(buffer + sigInLen, signature_, SIGNATURE_LEN);

        //hash (SHA-256) {central, scrypted_, signature_}
        Botan::SHA_256 sha256;
        auto hash = sha256.process(buffer, totalLen);

        //interpret hash output as number and compare against threshold
        auto num = Utils::arrayToUInt32(hash, 0);
        std::cout << Botan::base64_encode(nonce_, NONCE_LEN) << " -> " << num << std::endl;
        if (num < THRESHOLD)
        {
            valid_ = true;
            return true;
        }

        memset(signature_, 0, SIGNATURE_LEN);
        return false;
    }

    bool found = findNonce(depth + 1);
    if (found)
        return true;

    while (nonce_[depth] < UINT8_MAX)
    {
        nonce_[depth]++;
        found = findNonce(depth + 1);
        if (found)
            return true;
    }

    nonce_[depth] = 0;
    return false;
}
