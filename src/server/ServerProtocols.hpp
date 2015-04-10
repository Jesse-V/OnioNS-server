
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
      static std::shared_ptr<ServerProtocols> get();

      ServerProtocols();

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
      std::shared_ptr<boost::asio::io_service> service_;
      std::shared_ptr<tcp::acceptor> acceptor_;

      void start(int port);
      void stop();
      void accept();
      void handleAccept(std::shared_ptr<tcp::socket> pSocket);
      void serve(std::shared_ptr<tcp::socket> pSocket);
      std::string resolveDomain(const std::string&);

      static std::shared_ptr<ServerProtocols> singleton_;
};

#endif
