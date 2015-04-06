
#include "Environment.hpp"
#include <iostream>


Environment* Environment::singleton_ = 0;
Environment& Environment::get()
{
   if (singleton_)
      return *singleton_;

   singleton_ = new Environment();

   return *singleton_;
}
