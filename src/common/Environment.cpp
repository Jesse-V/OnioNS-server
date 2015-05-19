
#include "Environment.hpp"
#include <iostream>


void Environment::setQueryPipe(const std::string& pipePath)
{
  queryPath_ = pipePath;
}



std::string Environment::getQueryPipe()
{
  return queryPath_;
}



void Environment::setResponsePipe(const std::string& pipePath)
{
  responsePath_ = pipePath;
}



std::string Environment::getResponsePipe()
{
  return responsePath_;
}
