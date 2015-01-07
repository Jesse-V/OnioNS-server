
#include "Record.hpp"
#include <openssl/pem.h>
#include <cstring>


Record::Record()
{
    //memset(SALT, 0, SCRYPT_SALT_LEN);
}



int Record::signMessageDigest(const unsigned char* str, std::size_t strLen,
    RSA* key, uint8_t* sigOut)
{
    uint8_t digest[SHA256_DIGEST_LENGTH];
    SHA256(str, strLen, digest);

    return RSA_private_encrypt(SHA256_DIGEST_LENGTH, digest,
        sigOut, key, RSA_PKCS1_PADDING);
}



int Record::scrypt(const uint8_t* input, size_t inputLen, uint8_t* output)
{
    static uint8_t* const SALT = new uint8_t[SCRYPT_SALT_LEN];

    //RAM load = O(N * R)
    //CPU time = O(N * R * P)

    //mining on a quad-core: 256MB * 4 = 1 GB
    //BBB board has 512 MB of RAM, dual core CPU
    //modern desktop has 8 CPUs, 12 GB RAM, so mining takes 2 GB RAM
    //mining time is based on difficulty, its the verification that matters

    return libscrypt_scrypt(input, inputLen, SALT, SCRYPT_SALT_LEN,
        SCR_N, SCR_R, SCR_P, output, SCRYPTED_LEN);
}
