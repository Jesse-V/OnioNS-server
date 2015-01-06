
#ifndef MAIN
#define MAIN

#include <cstdint>
#include <string>
#include <openssl/rsa.h>

void mineRegistration(RSA*);
void recursiveMining(uint8_t*, uint8_t, RSA* key, uint8_t*, uint8_t*);
int signMessageDigest(const unsigned char*, std::size_t, RSA* key, uint8_t*);
int scrypt(const uint8_t*, size_t, uint8_t*);
uint64_t arrayToUInt64(const uint8_t*, int32_t);
void printAsHex(const uint8_t*, int);

#endif
