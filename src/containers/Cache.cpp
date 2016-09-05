
#include "Cache.hpp"
#include <algorithm>

std::vector<RecordPtr> Cache::records_;


bool Cache::add(const RecordPtr& record)
{  // todo: delete Records should cause deletion/replacement, etc
  if (get(record->getName()))
    return false;  // cannot add record, name is already taken

  records_.push_back(record);
  return true;
}



bool Cache::add(const std::vector<RecordPtr>& records)
{  // todo: push all at once
  bool allSucceeded = true;
  for (auto r : records)
    if (!add(r))
      allSucceeded = false;
  return allSucceeded;
}



std::vector<RecordPtr> Cache::getSortedList()
{
  std::sort(records_.begin(), records_.end(),
            [](const RecordPtr& a, const RecordPtr& b) {
              return a->getName().compare(b->getName());
            });
  return records_;
}



RecordPtr Cache::get(const std::string& name)
{
  for (auto r : records_)
  {
    // check main name
    if (r->getName() == name)
      return r;

    // check subdomain
    NameList list = r->getSubdomains();
    for (auto subdomain : list)
      if (subdomain.first + "." + r->getName() == name)
        return r;
  }

  return nullptr;
}



size_t Cache::getRecordCount()
{
  return records_.size();
}
