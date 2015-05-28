
#include "Cache.hpp"
#include <algorithm>


void Cache::add(const std::vector<RecordPtr>& records)
{  // todo: delete Records should cause deletion/replacement, etc
  for (auto r : records)
    records_.push_back(r);  // todo: push all at once
}



std::vector<RecordPtr> Cache::getSortedList()
{
  std::sort(records_.begin(), records_.end(),
            [](const RecordPtr& a, const RecordPtr& b)
            {
              return a->getName().compare(b->getName());
            });
  return records_;
}



RecordPtr Cache::get(const std::string& name)
{
  for (auto r : records_)
    if (r->getName() == name)
      return r;
  return nullptr;
}
