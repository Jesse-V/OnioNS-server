
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
};

#endif
