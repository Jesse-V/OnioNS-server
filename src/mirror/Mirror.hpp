
#ifndef MIRROR_HPP
#define MIRROR_HPP

#include "../common/records/Record.hpp"
#include <memory>
#include <string>

class Mirror
{
 public:
  static Mirror& get()
  {  // http://stackoverflow.com/questions/1008019/
    static Mirror instance;
    return instance;
  }

  // void initializeDatabase();
  // void validatePagechain();
  // void buildCache();

  void startServer();
  // void receiveNewRecord(const std::shared_ptr<Record>&);
  // void uploadPagechain();
  // void synchronizePagechain();
  // void publishDatabaseHash();
  // void selectPage();
  // void flood();

 private:
  Mirror() {}  // http://stackoverflow.com/questions/270947/
  Mirror(Mirror const&) = delete;
  void operator=(Mirror const&) = delete;
  static std::shared_ptr<Mirror> singleton_;
};

#endif
