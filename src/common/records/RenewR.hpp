
#ifndef RENEW_R_HPP
#define RENEW_R_HPP

#include "Record.hpp"
#include <string>

class RenewR: public Record
{
   public:
      virtual bool makeValid(uint8_t);
      virtual std::string asJSON() const;
      virtual uint32_t getDifficulty() const;
};

#endif
