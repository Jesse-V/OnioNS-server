
#ifndef RENEWAL_HPP
#define RENEWAL_HPP

#include "Record.hpp"
#include <string>

class Renewal: public Record
{
    public:
        virtual bool makeValid(uint8_t);
        virtual std::string asJSON() const;
        virtual uint32_t getDifficulty() const;
};

#endif
