
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
  static void openSocketTo(const std::string& host, ushort port);
  static std::string serverSendReceive(const std::string&, const std::string&);

  static std::vector<boost::shared_ptr<Session>> connections_;
  static boost::asio::io_service io_service;
  static boost::asio::ip::tcp::socket socket_;
};

#endif
