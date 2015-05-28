
#ifndef MIRROR_HPP
#define MIRROR_HPP

#include "../common/records/Record.hpp"

class Mirror
{
 public:
  static Mirror& get()
  {  // http://stackoverflow.com/questions/1008019/
    static Mirror instance;
    return instance;
  }

  void startServer();

 private:
  Mirror() {}  // http://stackoverflow.com/questions/270947/
  Mirror(Mirror const&) = delete;
  void operator=(Mirror const&) = delete;
};

#endif
