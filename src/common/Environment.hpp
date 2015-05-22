
#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <memory>
#include <string>

class Environment
{
 public:
  static Environment& get()
  {
    static Environment instance;
    return instance;
  }

 private:
  Environment() {}
  Environment(Environment const&) = delete;
  void operator=(Environment const&) = delete;
  static std::shared_ptr<Environment> singleton_;
};

#endif
