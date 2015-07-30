
#ifndef MIRROR_HPP
#define MIRROR_HPP

#include "tcp/Session.hpp"
#include <onions-common/containers/MerkleTree.hpp>
#include <onions-common/containers/records/Record.hpp>
#include <json/json.h>

class Mirror
{
 public:
  static void startServer(const std::string&, ushort, bool);
  static UInt8Array signMerkleRoot(Botan::RSA_PrivateKey*,
                                   const MerkleTreePtr&);

  static void addConnection(const boost::shared_ptr<Session>&);
  static void broadcastEvent(const std::string&, const Json::Value&);

 private:
  static void loadCache();
  static void subscribeToAuthority();
  static void receiveEvents();

  static std::vector<boost::shared_ptr<Session>> connections_;
  static std::shared_ptr<boost::asio::io_service> authIO_;
  static boost::shared_ptr<Session> authSession_;
  // static boost::asio::ip::tcp::socket authSocket_;
  // static boost::shared_ptr<Session> authSession_;
};

#endif
