
#ifndef MAIN
#define MAIN

#include <botan-1.10/botan/rsa.h>

Botan::RSA_PrivateKey* loadKey(const char* filename, Botan::RandomNumberGenerator& rng);
Botan::RSA_PrivateKey* loadOpenSSLRSA(const char* filename, Botan::RandomNumberGenerator& rng);

#endif
