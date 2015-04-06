
#ifndef COMMON_PROTOCOLS_HPP
#define COMMON_PROTOCOLS_HPP

#include "records/Record.hpp"
#include "Quorum.hpp"

class CommonProtocols
{
   public:
      static CommonProtocols& get();

      bool isRecordValid(const std::shared_ptr<Record>&);
      std::shared_ptr<Quorum> deriveQuorum();

   private:
      static Environment* singleton_;
};

#endif
