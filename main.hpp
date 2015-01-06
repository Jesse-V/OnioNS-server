
#ifndef MAIN
#define MAIN

#include <cstdint>

void mine(uint8_t* input, uint8_t depth, uint8_t* buffer);
int scrypt(uint8_t* input, uint8_t inputLen, uint8_t* output);
uint32_t arrayToUInt32(uint8_t* byteArray, int32_t offset);

#endif
