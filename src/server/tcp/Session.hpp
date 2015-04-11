
#ifndef SESSION_HPP
#define SESSION_HPP

#include <boost/asio.hpp>
#include "HandleAlloc.hpp"
#include <boost/enable_shared_from_this.hpp>
#include <string>

using boost::asio::ip::tcp;

class Session: public boost::enable_shared_from_this<Session>
{
   public:
      Session(boost::asio::io_service&);
      tcp::socket& getSocket();
      void Start();
      void handleRead(const boost::system::error_code&, size_t);
      void handleWrite(const boost::system::error_code&);

   private:
      tcp::socket socket_;
      boost::array<char, 1024> buffer_;
      HandleAlloc allocator_;
};

typedef boost::shared_ptr<Session> session_ptr;

#endif
