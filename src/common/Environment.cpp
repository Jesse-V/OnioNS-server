
#include "Environment.hpp"
#include <iostream>


std::shared_ptr<Environment> Environment::singleton_ = 0;
std::shared_ptr<Environment> Environment::get()
{
   if (singleton_)
      return singleton_;

   singleton_ = std::make_shared<Environment>();
   return singleton_;
}



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
