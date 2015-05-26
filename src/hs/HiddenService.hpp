
#ifndef HIDDEN_SERVICE_HPP
#define HIDDEN_SERVICE_HPP

#include "../common/records/Record.hpp"
#include <botan/rsa.h>
#include <memory>

class HiddenService
{
 public:
  static HiddenService& get()
  {
    static HiddenService instance;
    return instance;
  }

  std::shared_ptr<Record> createRecord();
  void broadcastRecord(const std::shared_ptr<Record>&);

 private:
  HiddenService() {}
  HiddenService(HiddenService const&) = delete;
  void operator=(HiddenService const&) = delete;
  static std::shared_ptr<HiddenService> singleton_;

  Botan::RSA_PrivateKey* loadKey();
};

#endif
