
#ifndef MERKLE_TREE_HPP
#define MERKLE_TREE_HPP

#include <onions-common/records/Record.hpp>
#include <onions-common/Constants.hpp>
#include <json/json.h>
#include <vector>
#include <memory>
#include <string>

class MerkleTree
{  // this tree is built and referenced from the leaves to the root

 public:
  MerkleTree(const std::vector<RecordPtr>&);
  Json::Value generateSubtree(const std::string&) const;
  static bool doesContain(const Json::Value&, const RecordPtr&);
  static SHA256_HASH extractRoot(const Json::Value&);
  SHA256_HASH getRootHash() const;

 private:
  class Node
  {
   public:
    typedef std::shared_ptr<MerkleTree::Node> NodePtr;

    Node();
    Node(const NodePtr&, const SHA256_HASH&);
    virtual ~Node() {}
    void setParent(const NodePtr&);
    void setChildren(const NodePtr&, const NodePtr&);
    NodePtr getParent() const;
    SHA256_HASH getHash() const;  // http://sphincs.cr.yp.to/
    std::string getBase64Hash() const;
    Json::Value asValue() const;

   protected:
    NodePtr parent_, leftChild_, rightChild_;
    SHA256_HASH hash_;
  };

  typedef std::shared_ptr<MerkleTree::Node> NodePtr;

  class Leaf : public Node
  {
   public:
    Leaf(const RecordPtr&, const NodePtr&);
    Leaf(const std::string&);
    std::string getName() const;

   private:
    std::string name_;
  };

  typedef std::shared_ptr<MerkleTree::Leaf> LeafPtr;

  SHA256_HASH buildTree(std::vector<NodePtr>&);
  static SHA256_HASH concatenateHashes(const NodePtr&, const NodePtr&);
  Json::Value generatePath(const LeafPtr&) const;
  Json::Value generateSpan(const LeafPtr&,
                           const std::vector<LeafPtr>::const_iterator&) const;

  static bool verifyPath(const Json::Value& value, const RecordPtr&);
  static bool verifySpan(const Json::Value& value, const RecordPtr&);

  static bool isLessThan(const LeafPtr&, const LeafPtr&);

  std::vector<LeafPtr> leaves_;
  SHA256_HASH rootHash_;
};

typedef std::shared_ptr<MerkleTree> MerkleTreePtr;

#endif
