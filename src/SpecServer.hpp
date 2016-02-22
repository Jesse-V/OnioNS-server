
#ifndef SPEC_SERVER_HPP
#define SPEC_SERVER_HPP

#include <onions-common/spec/AbstractSpecServer.h>

class SpecServer : public AbstractSpecServer
{
  public:
    SpecServer(jsonrpc::AbstractServerConnector& connector) :
      AbstractSpecServer(connector) {};

    virtual std::string getSpecVersion();
    virtual void putRecord(const Json::Value& record);
    virtual Json::Value getRecord(const std::string& domain);
    virtual std::string getSubtree(const std::string& domain);
    virtual Json::Value subscribe();
};

#endif
