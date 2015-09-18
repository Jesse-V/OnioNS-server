
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
  static Mirror& get()
  {
    static Mirror instance;
    return instance;
  }

  void startServer(const std::string&, ushort, bool);
  void addSubscriber(const boost::shared_ptr<Session>&);
  bool processNewRecord(const RecordPtr&);

  ED_SIGNATURE signMerkleRoot();
  Json::Value getRootSignature();
  static std::string getWorkingDir();

 private:
  Mirror() {}
  Mirror(Mirror const&) = delete;
  void operator=(Mirror const&) = delete;

  void resumeState();
  void loadPages();
  void loadKeyPair();
  ED_KEY loadSecretKey(const std::string&);
  void subscribeToQuorum(ushort);
  void receiveEvents(ushort);

  std::vector<boost::shared_ptr<Session>> subscribers_;
  boost::shared_ptr<Session> authSession_;
  std::shared_ptr<Page> page_;
  std::shared_ptr<MerkleTree> merkleTree_;
  std::pair<ED_KEY, ED_KEY> keypair_;
  bool isQuorumNode_;
};

#endif
