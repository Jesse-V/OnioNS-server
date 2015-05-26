
#ifndef MERKLE_NODE_HPP
#define MERKLE_NODE_HPP

#include "../records/Record.hpp"
#include "../Environment.hpp"
#include <memory>
#include <string>

class MerkleNode
{
  typedef std::shared_ptr<MerkleNode> NodePtr;

 public:
  MerkleNode(const NodePtr&, NodePtr&);
  MerkleNode(const std::shared_ptr<Record>&);

  uint8_t* getHash();
  bool isLeaf();

  NodePtr getLeftChild();
  NodePtr getRightChild();

  std::string getLeftBound();
  std::string getRightBound();

 private:
  uint8_t hash_[Environment::SHA384_LEN];
  NodePtr left_, right_;
  std::string leftBound_, rightBound_;
};

typedef std::shared_ptr<MerkleNode> NodePtr;

#endif
