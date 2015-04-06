
#include "utils.hpp"
#include <cstdio>


//https://stackoverflow.com/questions/6855115/byte-array-to-int-c
uint32_t Utils::arrayToUInt32(const uint8_t* byteArray, int32_t offset)
{
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



bool Utils::isPowerOfTwo(unsigned int x)
{ //glibc method of checking
   return ((x != 0) && !(x & (x - 1)));
}



//https://stackoverflow.com/questions/1494399/how-do-i-search-find-and-replace-in-a-standard-string
void Utils::stringReplace(std::string& str, const std::string& find,
   const std::string& replace)
{
   size_t pos = 0;
   while ((pos = str.find(find, pos)) != std::string::npos)
   {
      str.replace(pos, find.length(), replace);
      pos += replace.length();
   }
}
