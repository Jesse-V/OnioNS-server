
#ifndef MIRROR_HPP
#define MIRROR_HPP

#include "tcp/Session.hpp"
#include "Page.hpp"
#include <onions-common/containers/MerkleTree.hpp>
#include <onions-common/containers/records/Record.hpp>
#include <onions-common/tcp/AuthenticatedStream.hpp>
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
  void subscribeForRecords(const boost::shared_ptr<Session>&);
  bool processNewRecord(int, const RecordPtr&);
  void tellSubscribers(const RecordPtr&);

  std::string signTransmission(const Json::Value& value) const;
  Json::Value getRootSignature() const;
  std::string signMerkleRoot() const;
  ED_SIGNATURE fetchQuorumRootSignature();
  static std::string getWorkingDir();
  std::shared_ptr<MerkleTree> getMerkleTree() const;

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

  std::shared_ptr<AuthenticatedStream> qStream_;
  boost::shared_ptr<Session> qSession_;
  ED_SIGNATURE qRootSig_;
  std::shared_ptr<Page> page_;
  std::shared_ptr<MerkleTree> merkleTree_;
  std::vector<boost::shared_ptr<Session>> waitingForRecords_;
  std::pair<ED_KEY, ED_KEY> keypair_;
  bool isQuorumNode_;
};

#endif
