
#ifndef PAGE_HPP
#define PAGE_HPP

#include "records/Record.hpp"
#include <vector>
#include <string>

class Page
{
 private:
  std::vector<Record> recordList_;
};

#endif
