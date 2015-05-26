
#ifndef COMMON_HPP
#define COMMON_HPP

#include "records/Record.hpp"
#include "Quorum.hpp"
#include <json/json.h>
#include <memory>

class Common
{
 public:
  static Common& get()
  {
    static Common instance;
    return instance;
  }

  std::shared_ptr<Record> parseRecord(const std::string&);
  Json::Value toJSON(const std::string&);
  std::string getDestination(const std::shared_ptr<Record>&,
                             const std::string&);

  // bool isRecordValid(const std::shared_ptr<Record>&);
  // std::shared_ptr<Quorum> deriveQuorum();
  uint8_t* computeConsensusHash();

 private:
  Common() {}
  Common(Common const&) = delete;
  void operator=(Common const&) = delete;
  static std::shared_ptr<Common> singleton_;
};

#endif
