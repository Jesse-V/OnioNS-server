
#ifndef DOMAIN
#define DOMAIN

#include "Record.hpp"
#include <vector>
#include <string>
#include <ostream>

class Domain: public Record
{
    public:
        static const int DIFFICULTY = 3; //1/2^x chance of success, so order of magnitude
        static const uint32_t THRESHOLD = INT32_MAX / (1 << DIFFICULTY);

        Domain(const std::string&, uint8_t*, const std::string&, Botan::Private_Key*);
        ~Domain();

        bool setName(const std::string&);
        bool addSubdomain(const std::string&, const std::string&);
        bool setContact(const std::string&);
        bool setKey(Botan::Private_Key*);
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
        uint8_t* consensusHash_;
        std::string contact_;
        uint8_t* signature_;
        uint signatureLen_;
        uint8_t* nonce_; //uint8_t nonce_[NONCE_LEN];
        long timestamp_;
        Botan::Private_Key* key_;
        bool valid_;
};

#endif