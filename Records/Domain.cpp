
#include "Domain.hpp"
#include "../utils.hpp"
#include "../libs/base64.h"
#include <cstring>
#include <iostream>

/*
        uint8_t* signature_;
        uint8_t* nonce_;
        long timestamp_; //TODO: the number of seconds since epoch
*/

Domain::Domain(const std::string& name, uint8_t* consensusHash,
    const std::string& contact, RSA* key):
    name_(name), consensusHash_(consensusHash), contact_(contact),
    timestamp_(0), key_(key), valid_(false)
{
    nonce_ = new uint8_t[IN_SIZE];
    memset(nonce_, 0, IN_SIZE);
}



bool Domain::addSubdomain(const std::string& from, const std::string& to)
{
    if (subdomains_.size() >= 16 || from.size() > 32 || to.size() > 32)
        return false;

    subdomains_.push_back(std::make_pair(from, to));
    valid_ = false; //need new nonce now

    return true;
}



bool Domain::makeValid()
{
    uint8_t* scryptBuf = new uint8_t[Record::SCRYPTED_LEN];
    unsigned char* sigBuf = new unsigned char[1024];
    findNonce(0, scryptBuf, sigBuf);
    return true; //todo: if successful
}



bool Domain::isValid()
{
    return valid_;
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

        auto len = Record::signMessageDigest(
            reinterpret_cast<const unsigned char*>(nonce_), IN_SIZE, key_, sigBuf);
        //if (len < 0)
            //error

        if (Record::scrypt(sigBuf, len, scryptBuf) < 0)
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
