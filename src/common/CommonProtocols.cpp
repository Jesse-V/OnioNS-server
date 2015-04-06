
#include "CommonProtocols.hpp"


std::shared_ptr<CommonProtocols> CommonProtocols::singleton_ = 0;
std::shared_ptr<CommonProtocols> CommonProtocols::get()
{
   if (singleton_)
      return singleton_;

   singleton_ = std::make_shared<CommonProtocols>();
   return singleton_;
}
