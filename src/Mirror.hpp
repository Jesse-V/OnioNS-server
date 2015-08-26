
#ifndef MIRROR_HPP
#define MIRROR_HPP

#include "tcp/Session.hpp"
#include "Page.hpp"
#include <onions-common/containers/MerkleTree.hpp>
#include <onions-common/containers/records/Record.hpp>
#include <onions-common/tcp/TorStream.hpp>
#include <json/json.h>

class Mirror
{
 public:
  static void startServer(const std::string&, ushort, bool);
  static UInt8Array signMerkleRoot(Botan::RSA_PrivateKey*,
                                   const MerkleTreePtr&);

  static void addSubscriber(const boost::shared_ptr<Session>&);
  static void broadcastEvent(const std::string&, const Json::Value&);
  static std::pair<ED_KEY, ED_KEY> getKeys();

 private:
  static void loadPages();
  static void resumeState();
  static void subscribeToQuorum(ushort);
  static void receiveEvents(ushort);

  static std::vector<boost::shared_ptr<Session>> subscribers_;
  static boost::shared_ptr<Session> authSession_;
  static std::shared_ptr<Page> page_;
};

#endif
