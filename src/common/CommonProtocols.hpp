
#ifndef COMMON_PROTOCOLS_HPP
#define COMMON_PROTOCOLS_HPP

#include "records/Record.hpp"
#include "Quorum.hpp"
#include <memory>

class CommonProtocols
{
 public:
  static CommonProtocols& get()
  {
    static CommonProtocols instance;
    return instance;
  }

  bool isRecordValid(const std::shared_ptr<Record>&);
  std::shared_ptr<Quorum> deriveQuorum();
  uint8_t* computeConsensusHash();

 private:
  CommonProtocols() {}
  CommonProtocols(CommonProtocols const&) = delete;
  void operator=(CommonProtocols const&) = delete;
  static std::shared_ptr<CommonProtocols> singleton_;
};

#endif
