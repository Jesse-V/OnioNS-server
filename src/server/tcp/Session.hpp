
#ifndef SESSION_HPP
#define SESSION_HPP

#include <boost/asio.hpp>
#include "HandleAlloc.hpp"
#include <boost/enable_shared_from_this.hpp>
#include <string>

class Session: public boost::enable_shared_from_this<Session>
{
   public:
      Session(boost::asio::io_service&);
      boost::asio::ip::tcp::socket& getSocket();
      void start();
      void handleRead(const boost::system::error_code&, size_t);
      void handleWrite(const boost::system::error_code&);

   private:
      boost::asio::ip::tcp::socket socket_;
      boost::array<char, 1024> buffer_;
      HandleAlloc allocator_;
};

typedef boost::shared_ptr<Session> session_ptr;

#endif
