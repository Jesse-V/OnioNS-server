
#ifndef MIRROR_HPP
#define MIRROR_HPP

#include "tcp/Session.hpp"
#include <onions-common/containers/MerkleTree.hpp>
#include <onions-common/containers/records/Record.hpp>
#include <onions-common/tcp/TorStream.hpp>
#include <json/json.h>

class Mirror
{
 public:
  static void startServer(bool);
  static UInt8Array signMerkleRoot(Botan::RSA_PrivateKey*,
                                   const MerkleTreePtr&);

  static void addSubscriber(const boost::shared_ptr<Session>&);
  static void broadcastEvent(const std::string&, const Json::Value&);

 private:
  static void loadCache();
  static void subscribeToQuorum();
  static void receiveEvents();

  static std::vector<boost::shared_ptr<Session>> subscribers_;
  static boost::shared_ptr<Session> authSession_;
};

#endif
