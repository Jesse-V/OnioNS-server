
#ifndef MERKLE_TREE_HPP
#define MERKLE_TREE_HPP

#include "../../common/records/Record.hpp"
#include "../../common/Environment.hpp"
#include <vector>
#include <memory>
#include <string>

class MerkleTree
{
  class Node
  {
    typedef std::shared_ptr<MerkleTree::Node> NodePtr;

   public:
    Node(const NodePtr&, NodePtr&);
    Node(const RecordPtr&);

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

  typedef std::shared_ptr<MerkleTree::Node> NodePtr;

 public:
  // std::vector<NodePtr> getPathTo(const std::string&);
  // JSONObj merge(const std::vector<NodePtr>&, const std::vector<NodePtr>&);
  // void add(const std::vector<NodePtr>&);
  // void add(JSONObj range);

 private:
  NodePtr root_;
};

#endif
