
#ifndef CLIENT_PROTOCOLS_HPP
#define CLIENT_PROTOCOLS_HPP

#include "../common/records/Record.hpp"
#include "tcp/SocksClient.hpp"
#include <unordered_map>
#include <memory>
#include <string>

class ClientProtocols
{
   public:
      static ClientProtocols& get()
      {
         static ClientProtocols instance;
         return instance;
      }

      /*
         Accepts requests for .tor domains, resolves them, and write a .onion.
         Listens on an incoming named pipe, and writes to an outgoing named pipe.
      */
      void listenForDomains();


   private:
      ClientProtocols() {}
      ClientProtocols(ClientProtocols const&) = delete;
      void operator=(ClientProtocols const&) = delete;
      static std::shared_ptr<ClientProtocols> singleton_;

      std::string remotelyResolve(const std::string&);
      std::pair<int, int> establishIPC();
      bool connectToResolver();
      std::shared_ptr<SocksClient> remoteResolver_;
      std::unordered_map<std::string, std::string> cache_;
};

#endif
