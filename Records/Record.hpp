
#ifndef RECORD
#define RECORD

#include <openssl/rsa.h>
#include <cstdint>
#include <cmath>
#include "../libs/libscrypt-1.20/libscrypt.h"
#include "../libs/libscrypt-1.20/b64.h"


class Record
{
    public:
        static const uint64_t SCR_N = 20;//static_cast<uint64_t>(pow(2, 20));
        static const uint32_t SCR_R = 1;//static_cast<uint32_t>(pow(2, 1));
        static const uint32_t SCR_P = 0;//static_cast<uint32_t>(pow(2, 0));

        static const uint8_t SCRYPTED_LEN = 16;

        Record();
        static int signMessageDigest(const unsigned char*, std::size_t, RSA* key, uint8_t*);
        static int scrypt(const uint8_t*, size_t, uint8_t*);
};

#endif





