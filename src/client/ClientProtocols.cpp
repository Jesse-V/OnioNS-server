
#include "ClientProtocols.hpp"


ClientProtocols* ClientProtocols::singleton_ = 0;
ClientProtocols& ClientProtocols::get()
{
   if (singleton_)
      return *singleton_;

   singleton_ = new ClientProtocols();

   return *singleton_;
}
