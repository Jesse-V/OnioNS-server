
#ifndef CLIENT_PROTOCOLS_HPP
#define CLIENT_PROTOCOLS_HPP

#include "../common/records/Record.hpp"
#include "tcp/SocksClient.hpp"
#include <json/json.h>
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
      std::string resolve(const std::string&);

      std::shared_ptr<Record> parseRecord(const std::string&);
      Json::Value toJSON(const std::string&);
      std::string getDestination(const std::shared_ptr<Record>&, const std::string&);

   private:
      ClientProtocols() {}
      ClientProtocols(ClientProtocols const&) = delete;
      void operator=(ClientProtocols const&) = delete;
      static std::shared_ptr<ClientProtocols> singleton_;

      Botan::RSA_PublicKey* base64ToRSA(const std::string&);

      std::pair<int, int> establishIPC();
      bool connectToResolver();
      std::shared_ptr<SocksClient> remoteResolver_;
      std::unordered_map<std::string, std::string> cache_;
};

#endif
