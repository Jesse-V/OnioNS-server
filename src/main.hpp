
#ifndef MAIN
#define MAIN

#include <botan/rsa.h>

Botan::RSA_PrivateKey* loadKey(const char* filename, Botan::RandomNumberGenerator& rng);
Botan::RSA_PrivateKey* loadOpenSSLRSA(const char* filename, Botan::RandomNumberGenerator& rng);

#endif
