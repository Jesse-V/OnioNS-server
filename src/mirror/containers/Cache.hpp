
#ifndef CACHE_HPP
#define CACHE_HPP

#include "../../common/records/Record.hpp"
#include <vector>

class Cache
{
 public:
  static Cache& get()
  {
    static Cache instance;
    return instance;
  }

  bool add(const RecordPtr& record);
  bool add(const std::vector<RecordPtr>&);
  std::vector<RecordPtr> getSortedList();
  RecordPtr get(const std::string&) const;

 private:
  Cache() {}
  Cache(Cache const&) = delete;
  void operator=(Cache const&) = delete;

  std::vector<RecordPtr> records_;
};

#endif
