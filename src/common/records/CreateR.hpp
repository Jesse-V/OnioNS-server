
#ifndef CREATE_R_HPP
#define CREATE_R_HPP

#include "Record.hpp"
#include <vector>
#include <string>
#include <ostream>

class CreateR: public Record
{
   public:
      CreateR(Botan::RSA_PrivateKey*, uint8_t*, const std::string&, const std::string&);

      bool setName(const std::string&);
      bool addSubdomain(const std::string&, const std::string&);
      bool setContact(const std::string&);
      virtual bool makeValid(uint8_t);

      virtual std::string asJSON() const;
      virtual uint32_t getDifficulty() const;
      friend std::ostream& operator<<(std::ostream&, const CreateR&);

   private:
      virtual UInt32Data getCentral(uint8_t* nonce) const;

      std::string name_;
      std::vector<std::pair<std::string,std::string>> subdomains_;
      std::string contact_;
};

#endif
