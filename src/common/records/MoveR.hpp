
#ifndef MOVE_R_HPP
#define MOVE_R_HPP

#include "Record.hpp"
#include <string>

class MoveR: public Record
{
   public:
      //virtual bool makeValid(uint8_t);
      virtual std::string asJSON() const;
      virtual uint32_t getDifficulty() const;
};

#endif
