
#ifndef DOMAIN
#define DOMAIN

#include "Record.hpp"
#include <vector>
#include <string>
#include <ostream>

class Domain: public Record
{
    public:
        static const int DIFFICULTY = 0; //1/2^x chance of success, so order of magnitude
        static const uint32_t THRESHOLD = INT32_MAX / (1 << DIFFICULTY);

        Domain(const std::string&, uint8_t[SHA256_LEN],
            const std::string&, Botan::RSA_PrivateKey*);
        ~Domain();

        bool setName(const std::string&);
        bool addSubdomain(const std::string&, const std::string&);
        bool setContact(const std::string&);
        bool setKey(Botan::RSA_PrivateKey*);
        bool refresh();
        bool makeValid();
        bool isValid() const;
        std::string getOnion() const;
        std::pair<uint8_t*, size_t> asJSON() const;
        friend std::ostream& operator<<(std::ostream&, const Domain&);

    private:
        bool findNonce(uint8_t, uint8_t*);

        std::string name_;
        std::vector<std::pair<std::string,std::string>> subdomains_;
        uint8_t consensusHash_[SHA256_LEN];
        uint8_t signature_[SIGNATURE_LEN];
        uint8_t nonce_[NONCE_LEN];
        std::string contact_;
        long timestamp_;
        Botan::RSA_PrivateKey* key_;
        bool valid_;
};

#endif
