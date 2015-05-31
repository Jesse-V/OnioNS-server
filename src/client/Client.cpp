
#include "Client.hpp"
#include "tcp/IPC.hpp"
#include "../common/Common.hpp"
#include "../../Flags.hpp"
#include "../common/utils.hpp"
#include <json/json.h>
#include <iostream>


void Client::listenForDomains()
{
  // establish connection with remote resolver over Tor
  if (!connectToResolver())
    return;

  IPC ipc(Environment::IPC_PORT);
  ipc.start();
}



std::string Client::resolve(const std::string& torDomain)
{
  try
  {
    std::string domain = torDomain;

    while (Utils::strEndsWith(domain, ".tor"))
    {
      // check cache first
      auto iterator = cache_.find(domain);
      if (iterator == cache_.end())
      {
        std::cout << "Sending \"" << domain << "\" to name server...\n";
        auto received = socks_->sendReceive(domain);
        if (received.isMember("error"))
        {
          std::cerr << "Err: " << received["error"].asString() << std::endl;
          return "<Response_Error>";
        }
        else
          std::cout << "Received Record response." << std::endl;

        auto dest = Common::get().getDestination(
            Common::get().parseRecord(received["response"].asString()), domain);

        cache_[domain] = dest;
        domain = dest;
      }
      else
        domain = iterator->second;  // retrieve from cache
    }

    if (domain.length() != 22 || !Utils::strEndsWith(domain, ".onion"))
    {
      std::cerr << "Err: \"" + domain + "\" is not a HS address!" << std::endl;
      return "<Invalid_Result>";
    }

    return domain;
  }
  catch (std::runtime_error& re)
  {
    std::cerr << "Err: " << re.what() << std::endl;
  }

  return "<General_Error>";
}



// ***************************** PRIVATE METHODS *****************************



bool Client::connectToResolver()
{
  try
  {
    // connect over Tor to remote resolver
    std::cout << "Detecting the Tor Browser..." << std::endl;
    socks_ = std::make_shared<SocksClient>("localhost", 9150);
    socks_->connectTo(Flags::get().getMirrorIP(), Environment::SERVER_PORT);
    std::cout << "The Tor Browser appears to be running." << std::endl;

    std::cout << "Testing connection to the name server..." << std::endl;
    auto r = socks_->sendReceive("ping");
    if (r == "pong")
      std::cout << "Name server confirmed up." << std::endl;
    else
    {
      std::cout << r << std::endl;
      std::cerr << "Name server did not return a valid response!" << std::endl;
      return false;
    }
  }
  catch (boost::system::system_error const& ex)
  {
    std::cerr << ex.what() << std::endl;
    std::cerr << "Test failed. Cannot continue." << std::endl;
    return false;
  }

  return true;
}
