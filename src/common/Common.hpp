
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

  RecordPtr parseRecord(const std::string&);
  RecordPtr parseRecord(const Json::Value&);
  Json::Value toJSON(const std::string&);
  std::string getDestination(const RecordPtr&, const std::string&);

  uint8_t* computeConsensusHash();

 private:
  Common() {}
  Common(Common const&) = delete;
  void operator=(Common const&) = delete;

  RecordPtr assembleRecord(const Json::Value&);
  void checkValidity(const RecordPtr&);
};

#endif
