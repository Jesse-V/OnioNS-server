
#include "Domain.hpp"
#include "../utils.hpp"
#include <botan/base64.h>
#include <cstring>
#include <iostream>

#define SHA256_DIGEST_LENGTH 32 //todo: remove this

/*
    std::string name_;
    std::vector<std::pair<std::string,std::string>> subdomains_;
    uint8_t* consensusHash_;
    std::string contact_;
    uint8_t* signature_;
    uint signatureLen_;
    uint8_t* nonce_;
    long timestamp_;
    RSA* key_;
    bool valid_;
*/

Domain::Domain(const std::string& name, uint8_t* consensusHash,
    const std::string& contact, Botan::Private_Key* key):
    consensusHash_(consensusHash), signature_(0), signatureLen_(0),
    timestamp_(time(NULL)), valid_(false)
{
    setName(name);
    setContact(contact);
    setKey(key);

    nonce_ = new uint8_t[NONCE_LEN];
    memset(nonce_, 0, NONCE_LEN);
}



Domain::~Domain()
{
    delete signature_;
    delete nonce_;
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



bool Domain::setKey(Botan::Private_Key* key)
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
    signature_ = new unsigned char[1024];
    uint8_t* scryptBuf = new uint8_t[SCRYPTED_LEN];
    return findNonce(0, scryptBuf);
}



bool Domain::isValid() const
{
    return valid_;
}



std::string Domain::getOnion() const
{
    return "temp.onion"; //TODO: calculate hash
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

    str += "},\"cHash\":\"" + Botan::base64_encode(consensusHash_, SHA256_DIGEST_LENGTH);
    str += "\",\"pgp\":\"" + contact_;
    str += "\",\"sig\":\"" + Botan::base64_encode(signature_, signatureLen_);
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
    os << "   Authentication:" << std::endl;

    os << "      Nonce: ";
    if (dt.isValid())
        os << Utils::getAsHex(dt.nonce_, dt.NONCE_LEN) << std::endl;
    else
        os << "<regeneration required>" << std::endl;

    os << "      Day Consensus: " <<
        Botan::base64_encode(dt.consensusHash_, SHA256_DIGEST_LENGTH) << std::endl;

    os << "      Signature: ";
    if (dt.isValid())
        os << Botan::base64_encode(dt.signature_, dt.signatureLen_ / 4) << " ..." << std::endl;
    else
        os << "<regeneration required>" << std::endl;

    //os << "      PubKey: " << Botan::base64_encode(dt.key_.a, 256) << std::endl;

    return os;
}



bool Domain::findNonce(uint8_t depth, uint8_t* scryptBuf)
{
    if (depth > NONCE_LEN)
        return false;

    if (depth == NONCE_LEN)
    {
        //digitally sign (RSA-SHA256) the JSON-encoded record
        auto json = asJSON();
        signatureLen_ = signMessageDigest(json.first, json.second, key_, signature_);

        //pass signature through scrypt
        if (scrypt(signature_, signatureLen_, scryptBuf) < 0)
        {
            std::cout << "Error with scrypt call!" << std::endl;
            return false;
        }

        //interpret scrypt output as number and compare against threshold
        auto num = Utils::arrayToUInt32(scryptBuf, 0);
        std::cout << Botan::base64_encode(nonce_, NONCE_LEN) << " -> " << num << std::endl;
        if (num < THRESHOLD)
        {
            valid_ = true;
            return true;
        }

        signatureLen_ = 0;
        return false;
    }

    bool found = findNonce(depth + 1, scryptBuf);
    if (found)
        return true;

    while (nonce_[depth] < UINT8_MAX)
    {
        nonce_[depth]++;
        found = findNonce(depth + 1, scryptBuf);
        if (found)
            return true;
    }

    nonce_[depth] = 0;
    return false;
}
