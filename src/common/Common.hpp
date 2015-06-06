
#ifndef COMMON_HPP
#define COMMON_HPP

#include "records/Record.hpp"
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

  RecordPtr parseRecord(const std::string&) const;
  RecordPtr parseRecord(const Json::Value&) const;
  Json::Value toJSON(const std::string&) const;
  std::string getDestination(const RecordPtr&, const std::string&) const;

  uint8_t* computeConsensusHash() const;

 private:
  Common() {}
  Common(Common const&) = delete;
  void operator=(Common const&) = delete;

  RecordPtr assembleRecord(const Json::Value&) const;
  void checkValidity(const RecordPtr&) const;
};

#endif
