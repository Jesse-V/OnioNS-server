
#ifndef HS_PROTOCOLS
#define HS_PROTOCOLS

#include "../common/records/Record.hpp"
#include <botan/rsa.h>
#include <memory>

class HSProtocols
{
   public:
      static HSProtocols& get()
      {
         static HSProtocols instance;
         return instance;
      }

      std::shared_ptr<Record> createRecord();
      Botan::RSA_PrivateKey* loadKey();

   private:
      HSProtocols() {}
      HSProtocols(HSProtocols const&) = delete;
      void operator=(HSProtocols const&) = delete;
      static std::shared_ptr<HSProtocols> singleton_;
};

#endif
