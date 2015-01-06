
#include "main.hpp"
#include <cstring>
#include "libs/libscrypt-1.20/libscrypt.h"
#include "libs/libscrypt-1.20/b64.h"
#include "libs/base64.h"
#include <iostream>
#include <string>
#include <algorithm>


int main(int argc, char** argv)
{
    const uint8_t IN_SIZE = 32;
    uint8_t* input = new uint8_t[IN_SIZE];
    for (uint8_t j = 0; j < IN_SIZE; j++)
        input[j] = j;

    uint8_t* scrypted = new uint8_t[SCRYPT_HASH_LEN];
    scrypt(input, IN_SIZE, scrypted, SCRYPT_N);

    std::cout << base64_encode(scrypted, SCRYPT_HASH_LEN) << std::endl;

    return EXIT_SUCCESS;
}



int scrypt(uint8_t* input, uint8_t inputLen, uint8_t* output, uint64_t n)
{
    uint8_t* salt = new uint8_t[SCRYPT_SALT_LEN];
    memset(salt, 0, SCRYPT_SALT_LEN);

    return libscrypt_scrypt(input, inputLen, salt, SCRYPT_SALT_LEN,
        n, SCRYPT_r, SCRYPT_p, output, SCRYPT_HASH_LEN);
}
