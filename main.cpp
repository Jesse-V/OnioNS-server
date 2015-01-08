
#include "main.hpp"
#include "Records/Domain.hpp"
//#include <botan-1.10/botan/botan.h>
#include <botan-1.10/botan/rsa.h>
#include <botan-1.10/botan/sha2_32.h>
#include <iostream>

Botan::LibraryInitializer init;


int main(int argc, char** argv)
{
    try
    {
        Botan::AutoSeeded_RNG rng;
        auto pvtKey = Botan::PKCS8::load_key("/home/jesse/rsa.pem", rng);
        auto rsaKey = dynamic_cast<Botan::RSA_PrivateKey*>(pvtKey);
        if (!rsaKey)
            throw std::invalid_argument("The loaded key is not a RSA key!");

        Botan::SHA_256 sha;
        auto hash = sha.process("hello world");

        uint8_t cHash[32];
        memcpy(cHash, hash, 32);

        Domain d("example.tor", cHash, "AD97364FC20BEC80", rsaKey);
        std::cout << d;
        d.makeValid();
        std::cout << d;
    }
    catch (Botan::Decoding_Error& de)
    {
        std::cerr << de.what() << std::endl;
        std::cerr << "  Your key may not be formatted correctly." << std::endl;
        std::cerr << "  Use \"openssl pkcs8 -topk8 -nocrypt -in my_key.pem\" to convert the RSA private key into a standard format." << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << "\n";
    }

    return EXIT_SUCCESS;
}
