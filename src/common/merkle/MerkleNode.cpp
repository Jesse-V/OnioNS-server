
#include "MerkleNode.hpp"
#include <botan/sha2_64.h>


MerkleNode::MerkleNode(const NodePtr& leftChild, NodePtr& rightChild)
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



MerkleNode::MerkleNode(const std::shared_ptr<Record>& r)
    : left_(nullptr), right_(nullptr)
{
  // hash_ =
  // leftBound_
  // rightBound_
}



uint8_t* MerkleNode::getHash()
{
  return hash_;
}



bool MerkleNode::isLeaf()
{
  return !left_ && !right_;
}



NodePtr MerkleNode::getLeftChild()
{
  return left_;
}



NodePtr MerkleNode::getRightChild()
{
  return right_;
}



std::string MerkleNode::getLeftBound()
{
  return leftBound_;
}



std::string MerkleNode::getRightBound()
{
  return rightBound_;
}
