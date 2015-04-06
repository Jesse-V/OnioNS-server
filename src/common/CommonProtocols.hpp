
#ifndef COMMON_PROTOCOLS_HPP
#define COMMON_PROTOCOLS_HPP

#include "records/Record.hpp"
#include "Quorum.hpp"
#include <memory>

class CommonProtocols
{
   public:
      static std::shared_ptr<CommonProtocols> get();

      bool isRecordValid(const std::shared_ptr<Record>&);
      std::shared_ptr<Quorum> deriveQuorum();

   private:
      static std::shared_ptr<CommonProtocols> singleton_;
};

#endif
