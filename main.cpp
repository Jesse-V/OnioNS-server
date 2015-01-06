
#include "main.hpp"
#include <cstring>
#include "libs/libscrypt-1.20/libscrypt.h"
#include "libs/libscrypt-1.20/b64.h"
#include "libs/base64.h"
#include <iostream>
#include <cmath>
#include <string>
#include <chrono>
#include <thread>


//currently takes 256 MB and 1.3 seconds to do a scrypt on Core 2 Quad
const uint64_t N = static_cast<uint64_t>(pow(2, 20));
const uint32_t R = static_cast<uint32_t>(pow(2, 1));
const uint32_t P = static_cast<uint32_t>(pow(2, 0));


int main(int argc, char** argv)
{
    const uint8_t IN_SIZE = 32;
    uint8_t* input = new uint8_t[IN_SIZE];
    for (uint8_t j = 0; j < IN_SIZE; j++)
        input[j] = j;

    uint8_t* scrypted = new uint8_t[SCRYPT_HASH_LEN];

    std::cout << "Please take note of mem usage..." << std::endl;
    std::chrono::milliseconds duration(2000);
    std::this_thread::sleep_for(duration);

    std::cout << "Scrypting..." << std::endl;

    using namespace std::chrono;
    auto start = steady_clock::now();

    scrypt(input, IN_SIZE, scrypted);

    auto diff = duration_cast<milliseconds>(steady_clock::now() - start).count();
    std::cout << (diff / 1000.0f) << " seconds" << std::endl;

    std::cout << base64_encode(scrypted, SCRYPT_HASH_LEN) << std::endl;

    return EXIT_SUCCESS;
}



int scrypt(uint8_t* input, uint8_t inputLen, uint8_t* output)
{
    uint8_t* salt = new uint8_t[SCRYPT_SALT_LEN];
    memset(salt, 0, SCRYPT_SALT_LEN);

    //RAM load = O(N * R)
    //CPU time = O(N * R * P)

    //mining on a quad-core: 256MB * 4 = 1 GB
    //BBB board has 512 MB of RAM, dual core CPU
    //modern desktop has 8 CPUs, 12 GB RAM, so mining takes 2 GB RAM
    //mining time is based on difficulty, its the verification that matters

    return libscrypt_scrypt(input, inputLen, salt, SCRYPT_SALT_LEN,
        N, R, P, output, SCRYPT_HASH_LEN);
}
