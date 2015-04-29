
#include "CommonProtocols.hpp"
#include <botan/sha2_64.h>
#include <fstream>
#include <iostream>


uint8_t* CommonProtocols::computeConsensusHash()
{
   std::cout << "Reading network consensus... ";

   //https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
   std::fstream certsFile("/var/lib/tor-onions/cached-certs");
   std::fstream netStatFile("/var/lib/tor-onions/cached-microdesc-consensus");

   if (!certsFile)
      throw std::runtime_error("Cannot open consensus documents!");

   std::string certsStr((std::istreambuf_iterator<char>(certsFile)),
      std::istreambuf_iterator<char>());
   std::string netStatStr((std::istreambuf_iterator<char>(netStatFile)),
      std::istreambuf_iterator<char>());
   std::string consensusStr = certsStr + netStatStr;

   std::cout << "done. (" << consensusStr.length() << " bytes)" << std::endl;

   uint8_t* cHash = new uint8_t[48];
   Botan::SHA_384 sha;
   memcpy(cHash, sha.process(consensusStr), 48);

   return cHash;
}
