
#ifndef SESSION_HPP
#define SESSION_HPP

#include "../../common/tcp/HandleAlloc.hpp"
#include <json/json.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>

class Session : public boost::enable_shared_from_this<Session>
{
 public:
  Session(boost::asio::io_service&);
  boost::asio::ip::tcp::socket& getSocket();
  void start();

 private:
  void handlePing(Json::Value&, Json::Value&);
  void handleProveKnowledge(Json::Value&, Json::Value&);
  void handleUpload(Json::Value&, Json::Value&);
  void handleDomainQuery(Json::Value&, Json::Value&);

  void processRead(const boost::system::error_code&, size_t);
  void processWrite(const boost::system::error_code&);

  void asyncRead();
  void asyncWrite(const Json::Value&);
  void asyncWrite(const std::string&);

  boost::asio::ip::tcp::socket socket_;
  boost::array<char, 2048> buffer_;
  HandleAlloc allocator_;
};

#endif
