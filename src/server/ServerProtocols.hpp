
#ifndef SERVER_PROTOCOLS
#define SERVER_PROTOCOLS

#include "../common/records/Record.hpp"
#include <memory>
#include <string>

class ServerProtocols
{
   public:
      static ServerProtocols& get();

      void initializeDatabase();
      void validatePagechain();
      void buildCache();

      std::string resolveDomain(const std::string&);
      void receiveNewRecord(const std::shared_ptr<Record>&);
      void uploadPagechain();
      void synchronizePagechain();
      void publishDatabaseHash();
      void selectPage();
      void flood();

   private:
      static ServerProtocols* singleton_;
};

#endif
