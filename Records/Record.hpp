
#ifndef RECORD
#define RECORD

#include <openssl/rsa.h>
#include <cstdint>
#include "../libs/libscrypt-1.20/libscrypt.h"
#include "../libs/libscrypt-1.20/b64.h"

class Record
{
    public:
        //currently takes 256 MB. 1.3 secs on Core 2 Q9000, 0.7 secs on i7-2600k
        static const uint64_t SCR_N = 1 << 20;
        static const uint32_t SCR_R = 1 << 1;
        static const uint32_t SCR_P = 1 << 0;

        static const uint8_t NONCE_LEN = 4;
        static const uint8_t SCRYPTED_LEN = 16;

        static int signMessageDigest(const unsigned char*, std::size_t, RSA* key, uint8_t*);
        static int scrypt(const uint8_t*, size_t, uint8_t*);
};

#endif





