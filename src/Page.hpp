
#ifndef PAGE_HPP
#define PAGE_HPP

#include <onions-common/containers/records/Record.hpp>
#include <string>

class Page
{
 public:
  Page(const Json::Value&);
  Page(const SHA384_HASH&, const ED_KEY&);
  Page(const SHA384_HASH&, const SHA384_HASH&, const ED_KEY&);
  bool isValid(bool deepTest = false);
  void addRecord(const RecordPtr&);
  void resign(const std::array<uint8_t, Const::ED25519_KEY_LEN>&);
  Json::Value getCommonData() const;
  std::vector<RecordPtr> getRecords() const;
  SHA384_HASH toHash() const;
  std::string toString() const;
  // static Page selectPage(const std::vector<Page>&);

 private:
  SHA384_HASH prevHash_;  // linking
  SHA384_HASH rand_;      // current global random number
  ED_KEY publicEd_;       // owner's public key
  ED_SIGNATURE pageSig_;  // signature from owner
  std::vector<RecordPtr> recordList_;
};

#endif
