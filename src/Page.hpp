
#ifndef PAGE_HPP
#define PAGE_HPP

#include <onions-common/containers/records/Record.hpp>
#include <string>

class Page
{
 public:
  Page(const uint8_t*, const uint8_t*);  // rand_, fingerprint_
  Page(const uint8_t*,
       const uint8_t*,
       const uint8_t*);  // prevHash_, rand_, fingerprint_
  void addRecord(const RecordPtr&);
  void updateSignature(const uint8_t* pk);
  Json::Value getCommonData() const;
  uint8_t* toHash() const;
  std::string toString() const;
  // static Page selectPage(const std::vector<Page>&);

 private:
  const uint8_t* prevHash_;
  const uint8_t* rand_;
  const uint8_t* fingerprint_;
  uint8_t* pageSig_;
  std::vector<RecordPtr> recordList_;
};

#endif
