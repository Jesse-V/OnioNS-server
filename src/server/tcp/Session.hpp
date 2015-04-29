
#ifndef SESSION_HPP
#define SESSION_HPP

#include "HandleAlloc.hpp"
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>

class Session: public boost::enable_shared_from_this<Session>
{
   public:
      Session(boost::asio::io_service&);
      boost::asio::ip::tcp::socket& getSocket();
      void start();
      void processRead(const boost::system::error_code&, size_t);
      void processWrite(const boost::system::error_code&);

   private:
      void asyncReadBuffer();
      void asyncWriteBuffer(std::size_t);

      boost::asio::ip::tcp::socket socket_;
      boost::array<char, 2048> buffer_;
      HandleAlloc allocator_;
};

typedef boost::shared_ptr<Session> session_ptr;

#endif
