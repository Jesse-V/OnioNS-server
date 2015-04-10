
#include "ServerProtocols.hpp"


std::shared_ptr<ServerProtocols> ServerProtocols::singleton_ = 0;
std::shared_ptr<ServerProtocols> ServerProtocols::get()
{
   if (singleton_)
      return singleton_;

   singleton_ = std::shared_ptr<ServerProtocols>();
   return singleton_;
}



void ServerProtocols::listenForDomains()
{

}
