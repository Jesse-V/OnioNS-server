
#ifndef CLIENT_PROTOCOLS_HPP
#define CLIENT_PROTOCOLS_HPP

#include "../common/records/Record.hpp"
#include "tcp/SocksClient.hpp"
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
      std::string remotelyResolve(const std::string&);
      std::pair<int, int> establishIPC();
      bool connectToResolver();

      static std::shared_ptr<ClientProtocols> singleton_;
      std::shared_ptr<SocksClient> remoteResolver_;
};

#endif
