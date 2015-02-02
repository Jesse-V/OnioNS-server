
#ifndef RECORD
#define RECORD

#include <cstdint>
#include <botan-1.10/botan/botan.h>
#include <botan-1.10/botan/rsa.h>

class Record
{
    public:
        static const uint64_t SCR_N = 1 << 20;
        static const uint32_t SCR_P = 1 << 0;
            //RAM load = O(SCR_N)
            //CPU time = O(SCR_N * SCR_P)

        static const uint8_t NONCE_LEN = 4;
        static const uint8_t SCRYPTED_LEN = 16;

        static const uint32_t RSA_LEN = 1024;
        static const uint32_t SIGNATURE_LEN = RSA_LEN / 8;
        static const uint32_t SHA256_LEN = 256 / 8;
        static const uint32_t SHA1_LEN = 160 / 8;

        static size_t signMessageDigest(const uint8_t*, size_t,
            const Botan::Private_Key*, uint8_t*);
        static int scrypt(const uint8_t*, size_t, uint8_t*);
};

#endif





