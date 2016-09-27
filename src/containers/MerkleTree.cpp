
#include "MerkleTree.hpp"
#include <onions-common/Log.hpp>
#include <botan/sha2_64.h>
#include <botan/base64.h>


// records must be sorted by name
MerkleTree::MerkleTree(const std::vector<RecordPtr>& records)
{
  Log::get().notice("Building Merkle tree of size " +
                    std::to_string(records.size()));

  std::vector<NodePtr> row;
  for (auto r : records)
  {
    LeafPtr leaf = std::make_shared<Leaf>(r, nullptr);
    leaves_.push_back(leaf);
    row.push_back(leaf);
  }

  rootHash_ = buildTree(row);
  Log::get().notice("Built tree. Root is " +
                    Botan::base64_encode(rootHash_.data(), Const::SHA256_LEN));
}



Json::Value MerkleTree::generateSubtree(const std::string& domain) const
{
  if (leaves_.empty())
  {
    Json::Value empty;
    return empty;
  }

  LeafPtr needle = std::make_shared<Leaf>(domain);
  auto lowerBound =
      std::lower_bound(leaves_.begin(), leaves_.end(), needle, isLessThan);

  Log::get().notice("Lower bound on domain at " +
                    std::to_string(lowerBound - leaves_.begin()));

  Json::Value result;
  if (lowerBound != leaves_.end() && !isLessThan(needle, *lowerBound))
    result = generatePath(*lowerBound);  // found, so return single path
  else
    result = generateSpan(needle, lowerBound);  // not found, so return span

  return result;
}



// tests whether the record is contained within the subtree
bool MerkleTree::doesContain(const Json::Value& subtree,
                             const RecordPtr& record)
{
  if (subtree.isArray())
  {  // check main path
    if (!verifyPath(subtree, record))
      return false;
  }
  else
  {
    // https://stackoverflow.com/questions/1596668
    if (subtree.isMember("left") != subtree.isMember("right"))
      Log::get().warn("Malformed Merkle subtree, incomplete span.");

    if (subtree.isMember("left") || subtree.isMember("right"))
    {
      // verify each path's validity, then check if the span covers the Record
      if (!verifyPath(subtree["left"], record) ||
          !verifyPath(subtree["right"], record) || !verifySpan(subtree, record))
        return false;
    }
    else
      Log::get().warn("Subtree is missing both branches!");
  }

  return true;
}



SHA256_HASH MerkleTree::extractRoot(const Json::Value& subtree)
{
  std::string base64Root;

  // extract base64-encoded string
  if (subtree.isArray())
  {  // extract root from single path
    base64Root = subtree[subtree.size() - 1]["hash"].asString();
  }
  else
  {  // extract from a branch from the span

    // https://stackoverflow.com/questions/1596668
    if (subtree.isMember("left") != subtree.isMember("right"))
      Log::get().warn("Malformed Merkle subtree, incomplete span.");

    if (subtree.isMember("left"))
    {
      Json::Value leftPath = subtree["left"];
      base64Root = leftPath[leftPath.size() - 1]["hash"].asString();
    }
    else
      Log::get().warn("Subtree is missing both branches!");
  }

  SHA256_HASH root;
  if (Botan::base64_decode(root.data(), base64Root) != Const::SHA256_LEN)
    Log::get().warn("Invalid root size for Merkle subtree.");

  return root;
}



SHA256_HASH MerkleTree::getRootHash() const
{
  return rootHash_;
}



// ************************** PRIVATE METHODS **************************** //



SHA256_HASH MerkleTree::buildTree(std::vector<NodePtr>& row)
{
  // build breadth-first, row by row
  while (row.size() > 1)
  {
    std::vector<NodePtr> nextRow;
    for (size_t j = 0; j < row.size(); j += 2)
    {
      // get left and right, or get left twice if row size is odd
      NodePtr left = row[j + 0];
      NodePtr right = j + 1 < row.size() ? row[j + 1] : left;

      SHA256_HASH hash = concatenateHashes(left, right);
      NodePtr node = std::make_shared<Node>(nullptr, hash);

      nextRow.push_back(node);
      node->setChildren(left, j + 1 < row.size() ? right : nullptr);
      left->setParent(node);
      right->setParent(node);
    }

    row = nextRow;
  }

  return row[0]->getHash();
}



// returns a hash of the two nodes' values
SHA256_HASH MerkleTree::concatenateHashes(const NodePtr& a, const NodePtr& b)
{
  // hash their concatenation
  Botan::SHA_384 sha384;

  std::array<uint8_t, 2 * Const::SHA256_LEN> concat;
  memcpy(concat.data(), a->getHash().data(), Const::SHA256_LEN);
  memcpy(concat.data() + Const::SHA256_LEN, b->getHash().data(),
         Const::SHA256_LEN);

  SHA256_HASH result;
  auto hash = sha384.process(concat.data(), concat.size());
  memcpy(result.data(), hash, hash.size());

  Log::get().notice(std::to_string(hash.size()) + " | " +
                    std::to_string(Const::SHA256_LEN));

  return result;
}



Json::Value MerkleTree::generatePath(const LeafPtr& leaf) const
{
  Log::get().notice("Generating single path through Merkle tree.");

  Json::Value result;

  Json::Value leafVal;
  leafVal["name"] = leaf->getName();
  leafVal["hash"] = leaf->getBase64Hash();
  result.append(leafVal);

  NodePtr node = leaf->getParent();
  while (node)
  {
    result.append(node->asValue());
    node = node->getParent();
  }

  return result;
}



Json::Value MerkleTree::generateSpan(
    const LeafPtr& needle,
    const std::vector<LeafPtr>::const_iterator& lowerBound) const
{
  Log::get().notice("Generating span through Merkle tree.");

  auto upperBound =
      std::upper_bound(leaves_.begin(), leaves_.end(), needle, isLessThan);

  Json::Value lowerPath = generatePath(*lowerBound);
  Json::Value upperPath = generatePath(*upperBound);

  // todo: return "common" path where the two paths converge

  Json::Value result;
  result["left"] = lowerPath;
  result["right"] = upperPath;
  return result;
}



// checks the cryptographic validity of the paths to the Record
bool MerkleTree::verifyPath(const Json::Value& path, const RecordPtr& record)
{
  // check name
  if (path[0]["name"] != record->getName())
    return false;

  // check record's hash against first hash
  Leaf leaf(record, nullptr);
  if (path[0]["hash"] != leaf.getBase64Hash())
    return false;

  for (size_t j = 1; j < path.size(); j++)
  {
    // todo:
    // if previous hashes do not match either current match, return false
    // concatenateHashes
    // trouble is that I need the neighboring Record, yes? no
  }

  return true;
}



// checks whether a span bounds the Record
bool MerkleTree::verifySpan(const Json::Value& subtree, const RecordPtr& record)
{
  return subtree["left"][0]["name"].asString() < record->getName() &&
         record->getName() < subtree["right"][0]["name"].asString();
}



bool MerkleTree::isLessThan(const LeafPtr& a, const LeafPtr& b)
{
  return a->getName() < b->getName();
}



// ************************** SUBCLASS METHODS **************************** //



MerkleTree::Node::Node()
    : parent_(nullptr), leftChild_(nullptr), rightChild_(nullptr)
{
  hash_.fill(0);
}



MerkleTree::Node::Node(const NodePtr& parent, const SHA256_HASH& hash)
    : parent_(parent), leftChild_(nullptr), rightChild_(nullptr), hash_(hash)
{
}



void MerkleTree::Node::setParent(const NodePtr& parent)
{
  parent_ = parent;
}



void MerkleTree::Node::setChildren(const NodePtr& left, const NodePtr& right)
{
  leftChild_ = left;
  rightChild_ = right;
}



MerkleTree::NodePtr MerkleTree::Node::getParent() const
{
  return parent_;
}



SHA256_HASH MerkleTree::Node::getHash() const
{
  return hash_;
}



std::string MerkleTree::Node::getBase64Hash() const
{
  return Botan::base64_encode(hash_.data(), Const::SHA256_LEN);
}



Json::Value MerkleTree::Node::asValue() const
{
  Json::Value value;

  if (leftChild_)
    value["left"] = leftChild_->getBase64Hash();
  if (rightChild_)
    value["right"] = rightChild_->getBase64Hash();

  return value;
}



MerkleTree::Leaf::Leaf(const RecordPtr& record, const NodePtr& parent)
    : Node(parent, record->hash()), name_(record->getName())
{
}



MerkleTree::Leaf::Leaf(const std::string& name) : name_(name)
{
}



std::string MerkleTree::Leaf::getName() const
{
  return name_;
}
