
#include "Server.hpp"
#include <iostream>


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



void Server::start()
{
  std::cout << StartListening() << std::endl;
  getchar();
  StopListening();
}

