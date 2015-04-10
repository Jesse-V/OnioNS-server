
#ifndef SERVER_PROTOCOLS
#define SERVER_PROTOCOLS

#include "../common/records/Record.hpp"
#include <memory>
#include <string>

class ServerProtocols
{
   public:
      static std::shared_ptr<ServerProtocols> get();

      void initializeDatabase();
      void validatePagechain();
      void buildCache();

      void listenForDomains();
      void receiveNewRecord(const std::shared_ptr<Record>&);
      void uploadPagechain();
      void synchronizePagechain();
      void publishDatabaseHash();
      void selectPage();
      void flood();

   private:
      std::string resolveDomain(const std::string&);

      static std::shared_ptr<ServerProtocols> singleton_;
};

#endif
