
#ifndef SERVER_HPP
#define SERVER_HPP

#include <onions-common/api/NetworkServerInterface.h>
#include <onions-common/tor_ipc/TorController.hpp>
#include <onions-common/Constants.hpp>
//#include <onions-common/containers/MerkleTree.hpp>
//#include <onions-common/containers/records/Record.hpp>

class Server : public NetworkServerInterface
{
 public:
  struct ContactData
  {
    bool onDebugNetwork_;
    SHA256_HASH rootHash_;
    std::string addr_;
  };

  Server(jsonrpc::AbstractServerConnector&, short, short, bool);

  virtual void commitTicket(const std::string&, int);
  virtual void revealTicket(const Json::Value&, int);
  virtual void putRecord(const Json::Value&, int);
  virtual Json::Value getRecordWithSubtree(const std::string&, int);
  virtual std::string getDomain(const std::string&, int);
  virtual Json::Value getRecordsSince(int, int);

  bool updateContactData(TorController&,
                         bool,
                         const SHA256_HASH&,
                         const std::string&);
  bool updateContactString(TorController&, const std::string&);
  bool getContactData(const std::string&, ContactData&);
  std::string getHostname();

 private:
  bool qNode_;

  const unsigned long HS_ADDR_BYTES = 10;
  const unsigned long BIN_SIZE = Const::SHA256_LEN + HS_ADDR_BYTES;
};

#endif
