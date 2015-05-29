
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
    Node(const UInt8Array& value);
    Node(const NodePtr&, const NodePtr&, const NodePtr&, const UInt8Array&);
    ~Node();

    UInt8Array value_;
    NodePtr parent_, left_, right_;
  };

  typedef std::shared_ptr<MerkleTree::Node> NodePtr;

 public:
  MerkleTree(const std::vector<RecordPtr>&);
  // std::vector<NodePtr> getPathTo(const std::string&);
  // JSONObj merge(const std::vector<NodePtr>&, const std::vector<NodePtr>&);
  // void add(const std::vector<NodePtr>&);
  // void add(JSONObj range);
  // void getRoot();

 private:
  void fill(const std::vector<RecordPtr>&);
  void build();
  std::vector<NodePtr> buildParents(std::vector<NodePtr>&);
  uint8_t* join(const NodePtr&, const NodePtr&);
  UInt8Array concatenate(const NodePtr&, const NodePtr&);

  // tree is built bottom-up but can be accessed either way
  std::vector<std::pair<std::string, NodePtr>> leaves_;
  NodePtr root_;
};

#endif
