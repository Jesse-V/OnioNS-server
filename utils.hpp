
#ifndef UTILS
#define UTILS

#include <cstdint>

class Utils
{
    public:
        static uint32_t arrayToUInt32(const uint8_t*, int32_t);
        static char* getAsHex(const uint8_t*, int);
};

#endif
