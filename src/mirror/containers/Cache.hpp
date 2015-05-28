
#ifndef CACHE_HPP
#define CACHE_HPP

#include <vector>
#include <string>

class Cache
{
 public:
  static Cache& get()
  {
    static Cache instance;
    return instance;
  }

 private:
  Cache() {}
  Cache(Cache const&) = delete;
  void operator=(Cache const&) = delete;
};

#endif
