
#include "MerkleTree.hpp"
#include <botan/sha2_64.h>


MerkleTree::Node::Node(const NodePtr& leftChild, NodePtr& rightChild)
    : left_(leftChild),
      right_(rightChild),
      leftBound_(leftChild->leftBound_),
      rightBound_(rightChild->rightBound_)
{
  // concatenate the two hashes
  uint8_t* concat = new uint8_t[Environment::SHA384_LEN * 2];
  memcpy(concat, leftChild->hash_, Environment::SHA384_LEN);
  memcpy(concat + Environment::SHA384_LEN, rightChild->hash_,
         Environment::SHA384_LEN);

  Botan::SHA_384 sha;
  memcpy(hash_, sha.process(concat, Environment::SHA384_LEN * 2),
         Environment::SHA384_LEN);
  delete concat;
}



MerkleTree::Node::Node(const RecordPtr& r) : left_(nullptr), right_(nullptr)
{
  // hash_ =
  // leftBound_
  // rightBound_
}



uint8_t* MerkleTree::Node::getHash()
{
  return hash_;
}



bool MerkleTree::Node::isLeaf()
{
  return !left_ && !right_;
}



std::shared_ptr<MerkleTree::Node> MerkleTree::Node::getLeftChild()
{
  return left_;
}



std::shared_ptr<MerkleTree::Node> MerkleTree::Node::getRightChild()
{
  return right_;
}



std::string MerkleTree::Node::getLeftBound()
{
  return leftBound_;
}



std::string MerkleTree::Node::getRightBound()
{
  return rightBound_;
}
