
#include "MerkleTree.hpp"
#include <botan/sha2_64.h>
#include <botan/base64.h>
#include <json/json.h>
#include <iostream>

#define NodePtr std::shared_ptr<MerkleTree::Node>


// records must be in alphabetical order according to record.getName()
MerkleTree::MerkleTree(const std::vector<RecordPtr>& records)
{
  std::cout << "Building Merkle tree..." << std::endl;
  fill(records);
  build();
  std::cout << "Merkle tree root is "
            << Botan::base64_encode(root_->value_.first, root_->value_.second)
            << std::endl;
}



std::vector<NodePtr> MerkleTree::getPathTo(const std::string& name)
{
  std::vector<NodePtr> path;

  NodePtr node = getLeaf(name);
  if (!node)
    return path;

  while (node->parent_ != nullptr)
  {
    path.push_back(node);
    node = node->parent_;
  }

  std::reverse(path.begin(), path.end());
  return path;
}



uint8_t* MerkleTree::getRoot()
{
  return root_->value_.first;
}



// ************************** PRIVATE METHODS **************************** //



// builds leaves of tree from records
void MerkleTree::fill(const std::vector<RecordPtr>& records)
{
  for (auto r : records)
  {
    Json::Value val;
    Botan::SHA_384 sha;

    // encode authentication information as JSON
    val["name"] = r->getName();
    val["hash"] =
        Botan::base64_encode(sha.process(r->asJSON()), Environment::SHA384_LEN);

    Json::FastWriter writer;
    std::string json = writer.write(val);

    // save JSON as a binary array
    uint8_t* jsonBin = new uint8_t[json.size()];
    memcpy(jsonBin, json.c_str(), json.size());
    leaves_.push_back(std::make_pair(
        r->getName(), std::make_shared<MerkleTree::Node>(
                          std::make_pair(jsonBin, json.size()))));
  }
}



// builds the depth of the tree, sets pointers and root_
void MerkleTree::build()
{
  // convert to vector of NodePtrs
  std::vector<NodePtr> nodes;
  for (auto leaf : leaves_)
    nodes.push_back(leaf.second);

  // build each level until we reach the root
  while (nodes.size() > 1)
    nodes = buildParents(nodes);

  root_ = nodes[0];
}



// builds the level above the children, updates parent_ for the nodes
std::vector<NodePtr> MerkleTree::buildParents(std::vector<NodePtr>& nodes)
{
  std::vector<NodePtr> parents;

  for (ulong n = 0; n < nodes.size(); n += 2)
  {
    NodePtr left = nodes[n];
    NodePtr right = n + 1 < nodes.size() ? nodes[n + 1] : nullptr;

    // create parent node
    uint8_t* hash = join(left, right);
    auto hashLen = Environment::SHA384_LEN;
    NodePtr parent = std::make_shared<MerkleTree::Node>(
        nullptr, left, right, std::make_pair(hash, hashLen));
    parents.push_back(parent);

    // assign parent to children
    left->parent_ = parent;
    if (right)
      right->parent_ = parent;
  }

  std::cout << "Merkle level width: " << parents.size() << std::endl;

  return parents;
}



// returns a hash of the two nodes' values
uint8_t* MerkleTree::join(const NodePtr& a, const NodePtr& b)
{
  UInt8Array c = concatenate(a, b);

  // hash their concatenation
  Botan::SHA_384 sha;
  uint8_t* hash = new uint8_t[Environment::SHA384_LEN];
  memcpy(hash, sha.process(c.first, c.second), Environment::SHA384_LEN);
  return hash;
}



// concatenates the values of two nodes
UInt8Array MerkleTree::concatenate(const NodePtr& a, const NodePtr& b)
{
  auto aLen = a->value_.second;
  auto bLen = b ? b->value_.second : 0;
  uint8_t* concat = new uint8_t[aLen + bLen];

  memcpy(concat, a->value_.first, a->value_.second);
  if (b)
    memcpy(concat + a->value_.second, b->value_.first, b->value_.second);

  return std::make_pair(concat, aLen + bLen);
}



NodePtr MerkleTree::getLeaf(const std::string& name)
{  // todo: binary search
  for (auto l : leaves_)
    if (l.first == name)
      return l.second;
  return nullptr;
}



// ************************** TREE NODE METHODS **************************** //



MerkleTree::Node::Node(const UInt8Array& value)
    : Node(nullptr, nullptr, nullptr, value)
{
}



MerkleTree::Node::Node(const NodePtr& parent,
                       const NodePtr& left,
                       const NodePtr& right,
                       const UInt8Array& value)
    : value_(value), parent_(parent), left_(left), right_(right)
{
}



MerkleTree::Node::~Node()
{
  delete value_.first;
}
