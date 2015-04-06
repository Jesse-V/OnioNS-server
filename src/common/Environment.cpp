
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



void Environment::setReadPipe(const std::string& pipePath)
{
   readPipe_ = pipePath;
}



std::string Environment::getReadPipe()
{
   return readPipe_;
}



void Environment::setWritePipe(const std::string& pipePath)
{
   writePipe_ = pipePath;
}



std::string Environment::getWritePipe()
{
   return writePipe_;
}
