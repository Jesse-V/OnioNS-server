
#ifndef MERKLE_TREE_HPP
#define MERKLE_TREE_HPP

#include "MerkleNode.hpp"
#include <vector>
#include <memory>
#include <string>

class MerkleTree
{
 public:
  // std::vector<NodePtr> getPathTo(const std::string&);
  // JSONObj merge(const std::vector<NodePtr>&, const std::vector<NodePtr>&);
  // void add(const std::vector<NodePtr>&);
  // void add(JSONObj range);

 private:
  NodePtr root_;
};

#endif
