
#include "main.hpp"
#include <cstdlib>
#include "Records/Domain.hpp"
#include <openssl/pem.h>
#include <iostream>
#include <ctime>


int main(int argc, char** argv)
{
    FILE* rsaFile = fopen("/home/jesse/rsa2048.key", "r");
    if (rsaFile == NULL)
        perror("Error opening RSA keyfile");

    auto doc = "temp";
    uint8_t cHash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)doc, sizeof(doc), cHash);

    auto key = PEM_read_RSAPrivateKey(rsaFile, NULL, NULL, NULL);
    Domain d("example.tor", cHash, "0xAD97364FC20BEC80", key);
    //d.addSubdomain("sub", "example2.tor");

    std::cout << d;
    d.makeValid();
    d.asJSON();
    std::cout << d;

    return EXIT_SUCCESS;
}
