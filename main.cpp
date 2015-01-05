
#include "main.hpp"
#include <string>
#include <iostream>
#include "libs/libscrypt-1.20/libscrypt.h"


int main(int argc, char** argv)
{
    char* outbuf = new char[512];
    char* str = const_cast<char*>(std::string("hello world").c_str());
    libscrypt_hash(outbuf, str, SCRYPT_N, SCRYPT_r, SCRYPT_p);

    std::cout << "hash output: " << outbuf << std::endl;

    return EXIT_SUCCESS;
}
