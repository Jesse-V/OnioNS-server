
#ifndef CREATE_R_HPP
#define CREATE_R_HPP

#include "Record.hpp"
#include <vector>
#include <string>
#include <ostream>

class CreateR: public Record
{
   public:
      CreateR(Botan::RSA_PrivateKey*, const std::string&, const std::string&);

      void setNameList(const NameList&);
      NameList getNameList();
      void setContact(const std::string&);

      virtual bool makeValid(uint8_t);
      virtual std::string asJSON() const;
      virtual uint32_t getDifficulty() const;
      friend std::ostream& operator<<(std::ostream&, const CreateR&);

   private:
      virtual UInt32Data getCentral(uint8_t* nonce) const;

      NameList nameList_;
      std::string contact_;
};

#endif
