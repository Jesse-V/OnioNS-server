
#include "Record.hpp"
#include "../libs/libscrypt-1.20/libscrypt.h"
#include <botan-1.10/botan/pubkey.h>
#include <cstring>


size_t Record::signMessageDigest(const uint8_t* message, size_t length,
    const Botan::Private_Key* key, uint8_t* sigBuf)
{
    static Botan::AutoSeeded_RNG rng;

    //https://stackoverflow.com/questions/14263346/how-to-perform-asymmetric-encryption-with-botan
    //http://botan.randombit.net/manual/pubkey.html#signatures
    Botan::PK_Signer signer(*key, "EMSA-PSS(SHA-512)"); //EMSA4, the latest
    auto sig = signer.sign_message(message, length, rng);
    sig.copy(sigBuf, 1024);

    return sig.size();
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
