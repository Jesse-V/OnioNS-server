
#include "Domain.hpp"
#include "../utils.hpp"
#include "../libs/base64.h"
#include <cstring>
#include <iostream>


void Domain::mineRegistration(RSA* key)
{
    uint8_t* nonce = new uint8_t[IN_SIZE];
    memset(nonce, 0, IN_SIZE);

    uint8_t* scryptBuf = new uint8_t[Record::SCRYPTED_LEN];
    unsigned char* sigBuf = new unsigned char[1024];

    recursiveMining(nonce, 0, key, scryptBuf, sigBuf);
}



void Domain::recursiveMining(uint8_t* nonce, uint8_t depth,
    RSA* key, uint8_t* scryptBuf, uint8_t* sigBuf)
{
    if (depth < 0 || depth > IN_SIZE)
        return;

    if (depth == IN_SIZE)
    {
        //for (uint8_t n = 0; n < IN_SIZE; n++)
        //    std::cout << (int)nonce[n] << ' ';
        //std::cout << std::endl;

        std::cout << base64_encode(nonce, IN_SIZE) << std::endl;

        auto len = Record::signMessageDigest(reinterpret_cast<const unsigned char*>(nonce), IN_SIZE, key, sigBuf);
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

    recursiveMining(nonce, depth + 1, key, scryptBuf, sigBuf);
    while (nonce[depth] < UINT8_MAX)
    {
        nonce[depth]++;
        recursiveMining(nonce, depth + 1, key, scryptBuf, sigBuf);
    }

    nonce[depth] = 0;
}
