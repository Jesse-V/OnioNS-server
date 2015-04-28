
#include "ClientProtocols.hpp"
#include "../common/records/Registration.hpp"
#include "../common/Environment.hpp"
#include "../common/utils.hpp"
#include <sys/stat.h>
#include <thread>
#include <iostream>


void ClientProtocols::listenForDomains()
{
   const auto POLL_DELAY = std::chrono::milliseconds(500);
   const int MAX_LEN = 256;

   //establish connection with remote resolver over Tor
   if (!connectToResolver())
      return;

   //enable/get named pipes for Tor-OnioNS IPC
   auto pipes = establishIPC();
   int queryPipe = pipes.first, responsePipe = pipes.second;

   //prepare reading buffer
   char* buffer = new char[MAX_LEN + 1];
   memset(buffer, 0, MAX_LEN);

   std::cout << "Reading... " << std::endl;

   while (true)
   {
      //read .tor domain from Tor Browser
      ssize_t readLength = read(queryPipe, (void*)buffer, MAX_LEN);
      if (readLength < 0)
      {
         //std::cerr << "Read error from IPC named pipe!" << std::endl;
      }
      else if (readLength > 0)
      {
         std::cout << "Read " << readLength << " bytes from Tor." << std::endl;

         //resolve and flush result to Tor Browser
         auto onionOut = remotelyResolve(std::string(buffer, readLength - 1));

         std::cout << "Writing \"" << onionOut << "\" to Tor... ";
         ssize_t ret = write(responsePipe, onionOut.c_str(), onionOut.length() + 1);
         std::cout << "done, " << ret << std::endl;
      }

      //delay before polling pipe again
      std::this_thread::sleep_for(POLL_DELAY);
   }

   //tear down file descriptors
   close(queryPipe);
   close(responsePipe);
}



// *********************** PRIVATE METHODS: *********************



bool ClientProtocols::connectToResolver()
{
   try
   {
      std::cout << "Starting client functionality..." << std::endl;

      //connect over Tor to remote resolver
      remoteResolver_ = std::make_shared<SocksClient>("localhost", 9050);
      remoteResolver_->connectTo("129.123.7.8", 15678);
   }
   catch (boost::system::system_error const& ex)
   {
      std::cerr << ex.what() << std::endl;
      std::cerr << "Tor does not appear to be running! Aborting." << std::endl;
      return false;
   }
   catch (std::exception& ex)
   {
      std::cerr << ex.what() << std::endl;
      return false;
   }

   return true;
}



std::pair<int, int> ClientProtocols::establishIPC()
{
   //const auto PIPE_CHECK = std::chrono::milliseconds(500);
   const auto QUERY_PATH    = Environment::get().getQueryPipe();
   const auto RESPONSE_PATH = Environment::get().getResponsePipe();

   //create named pipes that we will use for Tor-OnioNS IPC
   std::cout << "Initializing IPC... ";
   mkfifo(QUERY_PATH.c_str(),    0777);
   mkfifo(RESPONSE_PATH.c_str(), 0777);
   std::cout << "done." << std::endl;

   //named pipes are best dealt with C-style
   //each side has to open for reading before the other can open for writing
   std::cout << "Waiting for Tor connection... ";
   std::cout.flush();
   int responsePipe = open(RESPONSE_PATH.c_str(), O_WRONLY);
   int queryPipe    = open(QUERY_PATH.c_str(),    O_RDONLY); //O_NONBLOCK
   std::cout << "done. " << std::endl;

   std::cout << "Listening on pipe \"" << QUERY_PATH  << "\" ..." << std::endl;
   std::cout << "Resolving to pipe \"" << RESPONSE_PATH << "\" ..." << std::endl;

   return std::make_pair(queryPipe, responsePipe);
}



std::string ClientProtocols::remotelyResolve(const std::string& domain)
{
   std::cout << "\"" << domain << "\" " << domain.size() << std::endl;
   std::string response = domain;

   try
   {
      while (Utils::strEndsWith(response, ".tor"))
      {
         std::cout << "Sending \"" << response << "\" to resolver..." << std::endl;

         response = remoteResolver_->sendReceive(response + "\r\n");
         if (response == "<MALFORMED>")
            return "";

         std::cout << "Resolved to \"" << response << "\"" << std::endl;
      }
   }
   catch (std::runtime_error& re)
   {
      std::cerr << "Err: " << re.what() << std::endl;
      return "<OnioNS_READFAIL>";
   }

   return response == domain ? "" : response;
}
