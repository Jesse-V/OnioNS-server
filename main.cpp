
#include "main.hpp"
#include <cstdlib>
#include "Records/Domain.hpp"
#include <openssl/pem.h>


int main(int argc, char** argv)
{
    FILE* rsaFile = fopen("/home/jesse/rsa2048.key", "r");
    if (rsaFile == NULL)
        perror ("Error opening RSA keyfile");

    Domain d;
    RSA* key = PEM_read_RSAPrivateKey(rsaFile, NULL, NULL, NULL);
    d.mineRegistration(key);

    return EXIT_SUCCESS;
}
