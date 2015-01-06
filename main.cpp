
#include "main.hpp"
#include <cstring>
#include "libs/libscrypt-1.20/libscrypt.h"
#include "libs/libscrypt-1.20/b64.h"
#include "libs/base64.h"
#include <iostream>
#include <cmath>
#include <cassert>
#include <chrono>
#include <thread>
#include <openssl/pem.h>


const uint8_t IN_SIZE = 8;
const uint8_t SCRYPTED_LEN = 16;

//currently takes 256 MB and 1.3 seconds to do a scrypt on Core 2 Quad
const uint64_t SCR_N = static_cast<uint64_t>(pow(2, 20));
const uint32_t SCR_R = static_cast<uint32_t>(pow(2, 1));
const uint32_t SCR_P = static_cast<uint32_t>(pow(2, 0));
const int DIFFICULTY = 5; //5, chance is ~1/32

uint8_t* const SALT = new uint8_t[SCRYPT_SALT_LEN];


int main(int argc, char** argv)
{
    memset(SALT, 0, SCRYPT_SALT_LEN);

    FILE* rsaFile = fopen("/home/jesse/rsa2048.key", "r");
    if (rsaFile == NULL)
        perror ("Error opening RSA keyfile");

    RSA* key = PEM_read_RSAPrivateKey(rsaFile, NULL, NULL, NULL);
    mineRegistration(key);

    return EXIT_SUCCESS;
}



void mineRegistration(RSA* key)
{
    uint8_t* nonce = new uint8_t[IN_SIZE];
    memset(nonce, 0, IN_SIZE);

    uint8_t* scryptBuf = new uint8_t[SCRYPTED_LEN];
    unsigned char* sigBuf = new unsigned char[1024];

    recursiveMining(nonce, 0, key, scryptBuf, sigBuf);
}



void recursiveMining(uint8_t* nonce, uint8_t depth,
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

        auto len = signMessageDigest(reinterpret_cast<const unsigned char*>(nonce), IN_SIZE, key, sigBuf);
        //if (len < 0)
            //error

        if (scrypt(sigBuf, len, scryptBuf) < 0)
            std::cout << "Error with scrypt call!" << std::endl;

        auto num = arrayToUInt64(scryptBuf, 0) ^ arrayToUInt64(scryptBuf, 4);
        if (num < UINT64_MAX / (uint8_t)pow(2, DIFFICULTY))
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



int signMessageDigest(const unsigned char* str, std::size_t strLen,
    RSA* key, uint8_t* sigOut)
{
    uint8_t digest[SHA256_DIGEST_LENGTH];
    SHA256(str, strLen, digest);

    return RSA_private_encrypt(SHA256_DIGEST_LENGTH, digest,
        sigOut, key, RSA_PKCS1_PADDING);
}



int scrypt(const uint8_t* input, size_t inputLen, uint8_t* output)
{
    //RAM load = O(N * R)
    //CPU time = O(N * R * P)

    //mining on a quad-core: 256MB * 4 = 1 GB
    //BBB board has 512 MB of RAM, dual core CPU
    //modern desktop has 8 CPUs, 12 GB RAM, so mining takes 2 GB RAM
    //mining time is based on difficulty, its the verification that matters

    return libscrypt_scrypt(input, inputLen, SALT, SCRYPT_SALT_LEN,
        SCR_N, SCR_R, SCR_P, output, SCRYPTED_LEN);
}



//https://stackoverflow.com/questions/6855115/byte-array-to-int-c
uint64_t arrayToUInt64(const uint8_t* byteArray, int32_t offset)
{
    assert((offset & 1) == 0); // Offset must be multiple of 2
    return *reinterpret_cast<const uint64_t*>(&byteArray[offset]);
}



void printAsHex(const uint8_t* data, int len)
{
    char* hexStr = new char[len * 2 + 1];

    for (int i = 0; i < len; i++)
        sprintf(&hexStr[i*2], "%02x", data[i]);
    printf("%s", hexStr);

    delete hexStr;
}
