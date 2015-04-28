
#ifndef DELETE_R_HPP
#define DELETE_R_HPP

#include "Record.hpp"
#include <string>

class DeleteR: public Record
{
   public:
      virtual bool makeValid(uint8_t);
      virtual std::string asJSON() const;
      virtual uint32_t getDifficulty() const;
};

#endif
