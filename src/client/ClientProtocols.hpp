
#ifndef CLIENT_PROTOCOLS_HPP
#define CLIENT_PROTOCOLS_HPP

#include "../common/records/Record.hpp"
#include <memory>
#include <string>

class ClientProtocols
{
   public:
      static std::shared_ptr<ClientProtocols> get();

      /*
         Accepts requests for .tor domains, resolves them, and write a .onion.
         Listens on an incoming named pipe, and writes to an outgoing named pipe.
      */
      void listenForDomains();
      std::shared_ptr<Record> generateRecord();
      void broadcastRecord(const std::shared_ptr<Record>&);

   private:
      void createNamedPipes();
      void pipeListen();
      std::string resolveByProxy(const std::string&);

      static std::shared_ptr<ClientProtocols> singleton_;
};

#endif
