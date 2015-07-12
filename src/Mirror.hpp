
#ifndef MIRROR_HPP
#define MIRROR_HPP

#include "tcp/Session.hpp"
#include <onions-common/containers/MerkleTree.hpp>
#include <onions-common/containers/records/Record.hpp>
#include <json/json.h>

class Mirror
{
 public:
  static void startServer(ushort, bool);
  static UInt8Array signMerkleRoot(Botan::RSA_PrivateKey*,
                                   const MerkleTreePtr&);

  static void addConnection(const std::shared_ptr<Session>&);
  static void broadcastEvent(const std::string&, const Json::Value&);

 private:
  static void loadCache();

  static std::vector<std::shared_ptr<Session>> connections_;
};

#endif
