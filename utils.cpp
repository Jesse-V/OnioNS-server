
#include "utils.hpp"
#include <cassert>
#include <cstdio>


//https://stackoverflow.com/questions/6855115/byte-array-to-int-c
uint32_t Utils::arrayToUInt32(const uint8_t* byteArray, int32_t offset)
{
    assert((offset & 1) == 0); // Offset must be multiple of 2
    return *reinterpret_cast<const uint32_t*>(&byteArray[offset]);
}



char* Utils::getAsHex(const uint8_t* data, int len)
{
    char* hexStr = new char[len * 2 + 3];
    hexStr[0] = '0';
    hexStr[1] = 'x';
    for (int i = 0; i < len; i++)
        sprintf(&hexStr[i * 2 + 2], "%02x", data[i]);
    return hexStr;
}
