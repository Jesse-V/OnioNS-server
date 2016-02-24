
#include "Server.hpp"
#include <spec/AbstractSpecClient.h>
#include <jsonrpccpp/client/connectors/socks5client.h>
#include <onions-common/Log.hpp>
#include <onions-common/Constants.hpp>
#include <chrono>
#include <thread>
#include <iostream>

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>

// ------------------------ spec implementation: ------------------------

std::string Server::getSpecVersion()
{
  return "1.0";
}



void Server::putRecord(const Json::Value& record)
{
  //todo
}



Json::Value Server::getRecord(const std::string& domain)
{
  //todo
  Json::Value temp;
  return temp;
}



std::string Server::getSubtree(const std::string& domain)
{
  //todo
  return "temp";
}



Json::Value Server::getRecordsSince(int time)
{
  //todo
  Json::Value temp;
  return temp;
}


// ------------------------ server meta methods: ------------------------
//https://github.com/Jesse-V/OnioNS-server/blob/a71420f833458b8848745f8334756a825bc55037/src/Mirror.cpp


void Server::start()
{
  //loadState();

  if (qNode_)
    Log::get().notice("Running as a Quorum server.");
  else
    Log::get().notice("Running as normal server.");
    
  if (!qNode_)
    pollQuorumAsync();
    
    
  Log::get().notice("Starting server.");
  while (StartListening())
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  Log::get().warn("Server aborted!");
  
  StopListening(); //close daemon
}



void Server::pollQuorumAsync()
{
  std::thread t([&]()
  {
    jsonrpc::Socks5Client torTunnel("localhost", std::to_string(socksPort_), "http://example.onion:" + Const::SERVER_PORT);
    AbstractSpecClient client(torTunnel);
    
    try
    {
      while (true)
      {
        Json::Value recordList = client.getRecordsSince(lastPoll_);
        
        using namespace std::chrono;
        auto epochT = system_clock::now().time_since_epoch();
        lastPoll_ = epochT.count() * system_clock::period::num / system_clock::period::den;
        
        std::this_thread::sleep_for(std::chrono::seconds(10));
      }
      
      /*
      std::cout << client.getData("key", 9) << std::endl;
      std::cout << client.basicGet() << std::endl;
      client.noArgNotification();
      Json::Value array;
      array.append(7);
      array.append(8);
      array.append(9);
      client.tellServer(array, false);*/
    }
    catch (jsonrpc::JsonRpcException e)
    {
      Log::get().warn(std::string("Client aborted! ") + e.what());
    }
  });
  t.detach();
}



std::string Server::getWorkingDir()
{
  std::string workingDir(getpwuid(getuid())->pw_dir);
  workingDir += "/.OnioNS/";

  if (mkdir(workingDir.c_str(), 0750) == 0)
    Log::get().notice("Working directory successfully created.");

  return workingDir;
}

