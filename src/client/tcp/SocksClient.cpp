
#include "SocksClient.hpp"
#include "SocksRequest.hpp"
#include "SocksReply.hpp"
#include <iostream>

using boost::asio::ip::tcp;


SocksClient::SocksClient(const std::string& socksIP, short socksPort)
    : ios_(std::make_shared<boost::asio::io_service>()),
      socket_(*ios_),
      resolver_(*ios_)
{
  std::cout << "Creating SOCKS connection..." << std::endl;
  tcp::resolver::query socks_query(socksIP, std::to_string(socksPort));
  tcp::resolver::iterator endpoint_iterator = resolver_.resolve(socks_query);
  boost::asio::connect(socket_, endpoint_iterator);
}



void SocksClient::connectTo(const std::string& host, short port)
{
  std::cout << "Resolving address of remote host..." << std::endl;
  tcp::resolver::query query(tcp::v4(), host, std::to_string(port));
  endpoint_ = *resolver_.resolve(query);

  if (!checkSOCKS())
    throw std::runtime_error("Remote host refused connection.");
}



Json::Value SocksClient::sendReceive(const std::string& output)
{
  std::cout << "Sending... ";
  std::cout.flush();

  // send as JSON
  Json::Value outVal;
  outVal["request"] = output;
  Json::FastWriter writer;
  boost::asio::write(socket_, boost::asio::buffer(writer.write(outVal)));

  // read from socket until newline
  std::cout << "receiving... ";
  std::cout.flush();
  boost::asio::streambuf response;
  boost::asio::read_until(socket_, response, "\n");

  // convert to string
  std::string responseStr;
  std::istream is(&response);
  is >> responseStr;

  // parse into JSON object
  Json::Reader reader;
  Json::Value responseVal;
  if (!reader.parse(responseStr, responseVal))
    responseVal["error"] = "Failed to parse response from server.";

  if (!responseVal.isMember("error") && !responseVal.isMember("response"))
    responseVal["error"] = "Invalid response from server.";

  std::cout << "done." << std::endl;
  std::cout.flush();

  return responseVal;
}


bool SocksClient::checkSOCKS()
{
  // std::cout << "Connecting via Tor..." << std::endl;

  SocksRequest sreq(SocksRequest::connect, endpoint_, "OnioNS");
  boost::asio::write(socket_, sreq.buffers());

  SocksReply srep;
  boost::asio::read(socket_, srep.buffers());

  if (!srep.success())
  {
    std::cerr << "Connection failed." << srep.status() << std::endl;
    return false;
  }

  return true;
}
