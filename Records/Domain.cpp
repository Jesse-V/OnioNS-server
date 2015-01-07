
#include "Domain.hpp"
#include "../utils.hpp"
#include "../libs/base64.h"
#include <openssl/sha.h>
#include <cstring>
#include <iostream>

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
    const std::string& contact, RSA* key):
    name_(name), consensusHash_(consensusHash), contact_(contact),
    signature_(0), signatureLen_(0), timestamp_(time(NULL)), key_(key), valid_(false)
{
    nonce_ = new uint8_t[IN_SIZE];
    memset(nonce_, 0, IN_SIZE);
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
    if (contactInfo.length() > 64)
        return false;

    contact_ = contactInfo;
    valid_ = false; //need new nonce now
    return true;
}



bool Domain::setKey(RSA* key)
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

    uint8_t* scryptBuf = new uint8_t[SCRYPTED_LEN];
    unsigned char* sigBuf = new unsigned char[1024];
    findNonce(0, scryptBuf, sigBuf);
    return true; //todo: if successful
}



bool Domain::isValid() const
{
    return valid_;
}



std::string Domain::getOnion() const
{
    return "temp.onion"; //TODO: calculate hash
}



/*
        RSA* key_;

std::pair<uint8_t*, size_t> asBinary()
{

}*/



std::ostream& operator<<(std::ostream& os, const Domain& dt)
{
    os << "Domain Registration: (currently " <<
        (dt.valid_ ? "valid)" : "invalid)") << std::endl;
    os << "   Name: " << dt.name_ << " -> " << dt.getOnion() << std::endl;
    os << "   Subdomains:";

    if (dt.subdomains_.empty())
        os << "(none)";
    else
        for (auto subd : dt.subdomains_)
            os << std::endl << "      " << subd.first << " -> " << subd.second;
    os << std::endl;

    os << "   Contact: " << dt.contact_ << std::endl;
    os << "   Time: " << dt.timestamp_ << std::endl;
    os << "   Authentication:" << std::endl;

    os << "      Nonce: ";
    if (dt.isValid())
        os << Utils::getAsHex(dt.nonce_, dt.NONCE_LEN);
    os << std::endl;

    os << "      Day Consensus: " <<
        base64_encode(dt.consensusHash_, SHA256_DIGEST_LENGTH) << std::endl;

    os << "      Signature: ";
    if (dt.isValid())
        os << base64_encode(dt.signature_, dt.signatureLen_);
    os << std::endl;

    //os << "      PubKey: " << base64_encode(dt.key_.a, 256) << std::endl;

    return os;
}



void Domain::findNonce(uint8_t depth, uint8_t* scryptBuf, uint8_t* sigBuf)
{
    if (depth < 0 || depth > IN_SIZE)
        return;

    if (depth == IN_SIZE)
    {
        //for (uint8_t n = 0; n < IN_SIZE; n++)
        //    std::cout << (int)nonce[n] << ' ';
        //std::cout << std::endl;

        std::cout << base64_encode(nonce_, IN_SIZE) << std::endl;

        auto len = signMessageDigest(
            reinterpret_cast<const unsigned char*>(nonce_), IN_SIZE, key_, sigBuf);
        //if (len < 0)
            //error

        if (scrypt(sigBuf, len, scryptBuf) < 0)
            std::cout << "Error with scrypt call!" << std::endl;

        auto num = Utils::arrayToUInt64(scryptBuf, 0) ^ Utils::arrayToUInt64(scryptBuf, 4);
        if (num < UINT64_MAX / (1 << DIFFICULTY))
        {
            std::cout << "      Found match!" << std::endl;
        }
    }

    findNonce(depth + 1, scryptBuf, sigBuf);
    while (nonce_[depth] < UINT8_MAX)
    {
        nonce_[depth]++;
        findNonce(depth + 1, scryptBuf, sigBuf);
    }

    nonce_[depth] = 0;
}
