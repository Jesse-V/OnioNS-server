
#ifndef SESSION_HPP
#define SESSION_HPP

#include <onions-common/tcp/HandleAlloc.hpp>
#include <json/json.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>

class Session : public boost::enable_shared_from_this<Session>
{
 public:
  Session(boost::asio::io_service&, int);
  ~Session();
  Json::Value respond(size_t);
  boost::asio::ip::tcp::socket& getSocket();

  void asyncRead();
  void asyncWrite(const std::string&, const std::string&);
  void asyncWrite(const Json::Value&);

 private:
  void respondToUpload(Json::Value&, Json::Value&);
  void respondToDomainQuery(Json::Value&, Json::Value&);
  void respondToSubscribe(Json::Value&, Json::Value&);

  void processRead(const boost::system::error_code&, size_t);
  void processWrite(const boost::system::error_code&);
  void asyncWrite(const std::string&);

  boost::asio::ip::tcp::socket socket_;
  boost::array<char, 1024> buffer_;
  HandleAlloc allocator_;
  int id_;
};

#endif
