
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

  static const uint32_t RSA_LEN = 1024;
  static const uint32_t SIGNATURE_LEN = RSA_LEN / 8;
  // static const uint32_t SHA256_LEN = 256 / 8;
  static const uint32_t SHA384_LEN = 384 / 8;
  static const uint32_t SHA1_LEN = 160 / 8;

 private:
  Environment() {}
  Environment(Environment const&) = delete;
  void operator=(Environment const&) = delete;
  static std::shared_ptr<Environment> singleton_;
};

#endif
