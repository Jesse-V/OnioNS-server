
#ifndef UTILS_HPP
#define UTILS_HPP

#include <botan/rsa.h>
#include <cstdint>
#include <string>

class Utils
{
 public:
  static uint32_t arrayToUInt32(const uint8_t*, int32_t);
  static char* getAsHex(const uint8_t*, int);
  static bool isPowerOfTwo(std::size_t);

  static unsigned long decode64Estimation(unsigned long);

  static void stringReplace(std::string&,
                            const std::string&,
                            const std::string&);
  static bool strEndsWith(const std::string& str, const std::string& ending);

  static Botan::RSA_PublicKey* base64ToRSA(const std::string&);
  static Botan::RSA_PrivateKey* loadKey(const std::string&,
                                        Botan::RandomNumberGenerator&);
  static Botan::RSA_PrivateKey* loadOpenSSLRSA(const std::string&,
                                               Botan::RandomNumberGenerator&);

  static void hex2bin(const char*, uint8_t*);
  static uint8_t char2int(const char);
};

#endif
