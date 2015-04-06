
#ifndef CLIENT_PROTOCOLS_HPP
#define CLIENT_PROTOCOLS_HPP

#include "../common/records/Record.hpp"
#include <memory>
#include <string>

class ClientProtocols
{
   public:
      static ClientProtocols& get();

      /*
         Accepts requests for .tor domains, resolves them, and write a .onion.
         Listens on an incoming named pipe, and writes to an outgoing named pipe.
      */
      std::string proxyDomainResolves(const std::string&);
      std::shared_ptr<Record> generateRecord();
      void broadcastRecord(const std::shared_ptr<Record>&);

   private:
      static ClientProtocols* singleton_;
};

#endif
