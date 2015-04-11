
#ifndef SERVER_PROTOCOLS
#define SERVER_PROTOCOLS

#include "../common/records/Record.hpp"
#include <boost/asio.hpp>
#include <memory>
#include <string>

using boost::asio::ip::tcp;

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

      std::shared_ptr<boost::asio::io_service> service_;
      std::shared_ptr<tcp::acceptor> acceptor_;

      void start(int port);
      void stop();
      void accept();
      void handleAccept(std::shared_ptr<tcp::socket> pSocket);
      void serve(std::shared_ptr<tcp::socket> pSocket);
      void resolve();
      std::string resolveDomain(const std::string&);

      static std::shared_ptr<ServerProtocols> singleton_;
};

#endif
