
#ifndef DELETION_HPP
#define DELETION_HPP

#include "Record.hpp"
#include <string>

class Deletion: public Record
{
    public:
        virtual bool makeValid(uint8_t);
        virtual std::string asJSON() const;
        virtual uint32_t getDifficulty() const;
};

#endif
