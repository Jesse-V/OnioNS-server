
#include "Record.hpp"
#include <openssl/pem.h>
#include <cstring>


int Record::signMessageDigest(const unsigned char* str, std::size_t strLen,
    RSA* key, uint8_t* sigOut)
{
    //generate SHA-256 digest
    uint8_t digest[SHA256_DIGEST_LENGTH];
    SHA256(str, strLen, digest);

    return RSA_private_encrypt(SHA256_DIGEST_LENGTH, digest,
        sigOut, key, RSA_PKCS1_PADDING);
}



int Record::scrypt(const uint8_t* input, size_t inputLen, uint8_t* output)
{
    //allocate and prepare static salt
    static uint8_t* const SALT = new uint8_t[SCRYPT_SALT_LEN];
    static bool saltReady = false;
    if (!saltReady)
    {
        memset(SALT, 0, SCRYPT_SALT_LEN);
        saltReady = true;
    }

    return libscrypt_scrypt(input, inputLen, SALT, SCRYPT_SALT_LEN,
        SCR_N, 1, SCR_P, output, SCRYPTED_LEN);
}
