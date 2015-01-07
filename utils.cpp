
#include "utils.hpp"
#include <cassert>
#include <cstdio>


//https://stackoverflow.com/questions/6855115/byte-array-to-int-c
uint64_t Utils::arrayToUInt64(const uint8_t* byteArray, int32_t offset)
{
    assert((offset & 1) == 0); // Offset must be multiple of 2
    return *reinterpret_cast<const uint64_t*>(&byteArray[offset]);
}



void Utils::printAsHex(const uint8_t* data, int len)
{
    char* hexStr = new char[len * 2 + 1];

    for (int i = 0; i < len; i++)
        sprintf(&hexStr[i*2], "%02x", data[i]);
    printf("%s", hexStr);

    delete hexStr;
}
