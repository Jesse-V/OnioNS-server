
#include "ServerProtocols.hpp"


ServerProtocols* ServerProtocols::singleton_ = 0;
ServerProtocols& ServerProtocols::get()
{
   if (singleton_)
      return *singleton_;

   singleton_ = new ServerProtocols();

   return *singleton_;
}
