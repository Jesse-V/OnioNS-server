
#ifndef HIDDEN_SERVICE_HPP
#define HIDDEN_SERVICE_HPP

#include "../common/records/Record.hpp"
#include <botan/rsa.h>
#include <memory>

class HS
{
 public:
  static HS& get()
  {
    static HS instance;
    return instance;
  }

  std::shared_ptr<Record> createRecord();
  void broadcastRecord(const std::shared_ptr<Record>&);

 private:
  HS() {}
  HS(HS const&) = delete;
  void operator=(HS const&) = delete;
  static std::shared_ptr<HS> singleton_;

  Botan::RSA_PrivateKey* loadKey();
};

#endif
