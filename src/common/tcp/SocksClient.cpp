
#include "SocksClient.hpp"
#include "SocksRequest.hpp"
#include "SocksReply.hpp"
#include "../Environment.hpp"
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



std::shared_ptr<SocksClient> SocksClient::getCircuitTo(const std::string& host)
{
  try
  {
    // connect over Tor to remote resolver
    std::cout << "Detecting the Tor Browser..." << std::endl;
    auto socks = std::make_shared<SocksClient>("localhost", 9150);
    socks->connectTo(host, Env::SERVER_PORT);
    std::cout << "The Tor Browser appears to be running." << std::endl;

    std::cout << "Testing connection to the server..." << std::endl;
    auto r = socks->sendReceive("ping", "");
    if (r == "pong")
    {
      std::cout << "Server confirmed up." << std::endl;
      return socks;
    }
    else
    {
      std::cout << r << std::endl;
      std::cerr << "Server did not return a valid response!" << std::endl;
      return nullptr;
    }
  }
  catch (boost::system::system_error const& ex)
  {
    std::cerr << ex.what() << std::endl;
    std::cerr << "Test failed. Cannot continue." << std::endl;
    return nullptr;
  }
}



Json::Value SocksClient::sendReceive(const std::string& type,
                                     const std::string& msg)
{
  std::cout << "Sending... ";
  std::cout.flush();

  // send as JSON
  Json::Value outVal;
  outVal["command"] = type;
  outVal["value"] = msg;
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



// ***************************** PRIVATE METHODS *****************************



void SocksClient::connectTo(const std::string& host, short port)
{
  std::cout << "Resolving address of remote host..." << std::endl;
  tcp::resolver::query query(tcp::v4(), host, std::to_string(port));
  endpoint_ = *resolver_.resolve(query);

  if (!checkConnection())
    throw std::runtime_error("Remote host refused connection.");
}



bool SocksClient::checkConnection()
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
