
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../common/tcp/SocksClient.hpp"
#include <botan/rsa.h>
#include <unordered_map>
#include <memory>
#include <string>

class Client
{
 public:
  static Client& get()
  {
    static Client instance;
    return instance;
  }

  /*
     Accepts requests for .tor domains, resolves them, and write a .onion.
     Listens on an incoming named pipe, and writes to an outgoing named pipe.
  */
  void listenForDomains();
  std::string resolve(const std::string&);

 private:
  Client() {}
  Client(Client const&) = delete;
  void operator=(Client const&) = delete;
  bool connectToResolver();

  std::shared_ptr<SocksClient> socks_;
  std::unordered_map<std::string, std::string> cache_;
};

#endif
