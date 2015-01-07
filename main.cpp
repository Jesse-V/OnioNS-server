
#include "main.hpp"
#include <cstdlib>
#include "Records/Domain.hpp"
#include <openssl/pem.h>


int main(int argc, char** argv)
{
    FILE* rsaFile = fopen("/home/jesse/rsa2048.key", "r");
    if (rsaFile == NULL)
        perror("Error opening RSA keyfile");

    auto cHash = "temp";
    auto key = PEM_read_RSAPrivateKey(rsaFile, NULL, NULL, NULL);
    Domain d("example.tor", (uint8_t*)cHash, "0xAD97364FC20BEC80", key);
    d.makeValid();

    return EXIT_SUCCESS;
}
