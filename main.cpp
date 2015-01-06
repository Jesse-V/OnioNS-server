
#include "main.hpp"
#include <cstring>
#include "libs/libscrypt-1.20/libscrypt.h"
#include "libs/libscrypt-1.20/b64.h"
#include "libs/base64.h"
#include <iostream>
#include <cmath>
#include <cassert>
#include <string>
#include <chrono>
#include <thread>

const uint8_t IN_SIZE = 4; //32

//currently takes 256 MB and 1.3 seconds to do a scrypt on Core 2 Quad
const uint64_t N = static_cast<uint64_t>(pow(2, 20));
const uint32_t R = static_cast<uint32_t>(pow(2, 1));
const uint32_t P = static_cast<uint32_t>(pow(2, 0));

uint8_t* const SALT = new uint8_t[SCRYPT_SALT_LEN];

using namespace std::chrono;
auto start = steady_clock::now();


int main(int argc, char** argv)
{
    memset(SALT, 0, SCRYPT_SALT_LEN);

    uint8_t* input = new uint8_t[IN_SIZE];
    memset(input, 0, IN_SIZE);

    uint8_t* scrypted = new uint8_t[SCRYPT_HASH_LEN];
    mine(input, 0, scrypted);

    return EXIT_SUCCESS;
}



void mine(uint8_t* input, uint8_t depth, uint8_t* buffer)
{
    if (depth < 0 || depth > IN_SIZE)
        return;

    if (depth == IN_SIZE)
    {
        //for (uint8_t n = 0; n < IN_SIZE; n++)
        //    std::cout << (int)input[n] << ' ';
        //std::cout << std::endl;

        std::cout << base64_encode(input, IN_SIZE) << std::endl;
        scrypt(input, IN_SIZE, buffer);
        auto num = arrayToUInt32(buffer, 0);

        if (num < UINT32_MAX / (uint8_t)pow(2, 5))
        {
            auto diff = duration_cast<milliseconds>(steady_clock::now() - start).count();
            std::cout << "      Found match! Time elapsed: " <<
                (diff / 1000.0f) << " seconds" << std::endl;
        }

    }

    mine(input, depth + 1, buffer);
    while (input[depth] < UINT8_MAX)
    {
        input[depth]++;
        mine(input, depth + 1, buffer);
    }

    input[depth] = 0;
}



int scrypt(uint8_t* input, uint8_t inputLen, uint8_t* output)
{
    //RAM load = O(N * R)
    //CPU time = O(N * R * P)

    //mining on a quad-core: 256MB * 4 = 1 GB
    //BBB board has 512 MB of RAM, dual core CPU
    //modern desktop has 8 CPUs, 12 GB RAM, so mining takes 2 GB RAM
    //mining time is based on difficulty, its the verification that matters

    return libscrypt_scrypt(input, inputLen, SALT, SCRYPT_SALT_LEN,
        N, R, P, output, SCRYPT_HASH_LEN);
}



//https://stackoverflow.com/questions/6855115/byte-array-to-int-c
uint32_t arrayToUInt32(uint8_t* byteArray, int32_t offset)
{
    assert((offset & 1) == 0); // Offset must be multiple of 2
    return *(uint32_t*)&byteArray[offset];
}
