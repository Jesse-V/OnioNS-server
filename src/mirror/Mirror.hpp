
#ifndef MIRROR_HPP
#define MIRROR_HPP

#include "../common/records/Record.hpp"
#include "containers/MerkleTree.hpp"
#include "tcp/Session.hpp"
#include <json/json.h>

class Mirror
{
 public:
  static Mirror& get()
  {  // http://stackoverflow.com/questions/1008019/
    static Mirror instance;
    return instance;
  }

  void startServer();
  UInt8Array signMerkleRoot(Botan::RSA_PrivateKey*, const MerkleTreePtr&) const;

  void addConnection(const std::shared_ptr<Session>&);
  void broadcastEvent(const std::string&, const Json::Value&);

 private:
  Mirror() {}  // http://stackoverflow.com/questions/270947/
  Mirror(Mirror const&) = delete;
  void operator=(Mirror const&) = delete;

  void loadCache() const;

  std::vector<std::shared_ptr<Session>> connections_;
};

#endif
