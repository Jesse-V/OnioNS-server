
#ifndef HIDDEN_SERVICE_HPP
#define HIDDEN_SERVICE_HPP

#include "../common/records/Record.hpp"
#include <botan/rsa.h>

class HS
{
 public:
  static HS& get()
  {
    static HS instance;
    return instance;
  }

  RecordPtr createRecord();
  RecordPtr promptForRecord();
  // void broadcastRecord(const RecordPtr&);

 private:
  HS() {}
  HS(HS const&) = delete;
  void operator=(HS const&) = delete;

  Botan::RSA_PrivateKey* loadKey();
};

#endif
