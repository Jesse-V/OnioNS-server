
#ifndef SERVER_PROTOCOLS
#define SERVER_PROTOCOLS

#include "../common/records/Record.hpp"
#include <memory>
#include <string>

class ServerProtocols
{
   public:
      static ServerProtocols& get()
      { //http://stackoverflow.com/questions/1008019/
         static ServerProtocols instance;
         return instance;
      }

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
      ServerProtocols(); //http://stackoverflow.com/questions/270947/
      ServerProtocols(ServerProtocols const&) = delete;
      void operator=(ServerProtocols const&) = delete;

      static std::shared_ptr<ServerProtocols> singleton_;
};

#endif
