
#ifndef DOMAIN
#define DOMAIN

#include "Record.hpp"
#include <vector>
#include <string>

class Domain
{
    public:
        static const uint8_t IN_SIZE = 8;
        static const int DIFFICULTY = 3; //1/2^x chance of success, so order of magnitude
        //difficulty at 17:
            //4 days on Q9000 @ 1 CPU, 256MB RAM
            //1 day on Q9000 @ 4 CPUs, 1GB RAM
            //1 day on i7-2600k @ 1 CPU, 256MB RAM
            //6 hours on i7-2600k @ 8 CPUs, 2GB RAM

        Domain(const std::string&, uint8_t*, const std::string&, RSA*);
        bool addSubdomain(const std::string& from, const std::string& to);
        bool makeValid();
        bool isValid();

    private:
        void findNonce(uint8_t, uint8_t*, uint8_t*);

        std::string name_;
        std::vector<std::pair<std::string,std::string>> subdomains_;
        uint8_t* consensusHash_;
        std::string contact_;
        uint8_t* signature_;
        uint8_t* nonce_;
        long timestamp_;
        RSA* key_;
        bool valid_;
};

#endif
