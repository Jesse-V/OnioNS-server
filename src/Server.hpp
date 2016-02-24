
#ifndef SERVER_HPP
#define SERVER_HPP

#include <onions-common/spec/AbstractSpecServer.h>
//#include <onions-common/containers/MerkleTree.hpp>
//#include <onions-common/containers/records/Record.hpp>

class Server : public AbstractSpecServer
{
  public:
    Server(jsonrpc::AbstractServerConnector& connector, short socksPort, bool qNode) :
      AbstractSpecServer(connector), socksPort(socksPort), qNode(qNode)
    {};

    virtual std::string getSpecVersion();
    virtual void putRecord(const Json::Value& record);
    virtual Json::Value getRecord(const std::string& domain);
    virtual std::string getSubtree(const std::string& domain);
    virtual Json::Value getRecordsSince(int time);
    
    void start();
    
  private:
    short socksPort;
    bool qNode;
};

#endif
