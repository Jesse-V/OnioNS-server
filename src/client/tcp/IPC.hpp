
#ifndef IPC_HPP
#define IPC_HPP

#include "IPCSession.hpp"
#include <boost/asio.hpp>
#include <string>

class IPC
{
 public:
  IPC(ushort);
  ~IPC();
  void start();

 private:
  void handleAccept(std::shared_ptr<IPCSession>,
                    const boost::system::error_code&);

  std::shared_ptr<boost::asio::io_service> ios_;
  boost::asio::ip::tcp::acceptor acceptor_;
};

#endif
