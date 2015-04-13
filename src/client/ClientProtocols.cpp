
#include "ClientProtocols.hpp"
#include "../common/records/Registration.hpp"
#include "../common/Environment.hpp"
#include "../common/utils.hpp"
#include <botan/sha2_32.h>
#include <sys/stat.h>
//#include <fcntl.h>
//#include <unistd.h>
#include <thread>
#include <iostream>


std::shared_ptr<ClientProtocols> ClientProtocols::singleton_ = 0;
std::shared_ptr<ClientProtocols> ClientProtocols::get()
{
   if (singleton_)
      return singleton_;

   singleton_ = std::make_shared<ClientProtocols>();
   return singleton_;
}



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
      int readLength = read(queryPipe, (void*)buffer, MAX_LEN);
      if (readLength < 0)
      {
         //std::cerr << "Read error from IPC named pipe!" << std::endl;
      }
      else if (readLength > 0)
      {
         std::cout << "Read " << readLength << " bytes from Tor." << std::endl;

         //terminate buffer, convert read to string
         buffer[readLength] = '\0';
         std::string domainIn(buffer, readLength - 1);

         //resolve and flush result to Tor Browser
         auto onionOut = remotelyResolve(domainIn);
         std::cout << "Writing \"" << onionOut << "\" to Tor... ";
         int ret = write(responsePipe, (onionOut+"\0").c_str(), onionOut.length() + 1);
         std::cout << "done, " << ret << std::endl;
      }

      //delay before polling pipe again
      std::this_thread::sleep_for(POLL_DELAY);
   }

   //tear down file descriptors
   close(queryPipe);
   close(responsePipe);
}



std::shared_ptr<Record> ClientProtocols::generateRecord()
{
   try
   {
      Botan::AutoSeeded_RNG rng;
      Botan::RSA_PrivateKey* rsaKey = Utils::loadKey("assets/example.key", rng);
      if (rsaKey != NULL)
         std::cout << "RSA private key loaded successfully!" << std::endl;

      Botan::SHA_256 sha;
      auto hash = sha.process("hello world");

      uint8_t cHash[32];
      memcpy(cHash, hash, 32);

      auto r = std::make_shared<Registration>(rsaKey, cHash,
         "example.tor", "AD97364FC20BEC80");

      std::cout << std::endl;
      std::cout << "Initial JSON: " << r->asJSON() << std::endl;

      r->makeValid(4);

      std::cout << std::endl;
      std::cout << "Result:" << std::endl;
      std::cout << r << std::endl;

      std::cout << std::endl;
      std::cout << "Final JSON: " << r->asJSON() << std::endl;

      return r;
   }
   catch (std::exception& e)
   {
      std::cerr << e.what() << "\n";
   }

   return NULL;

/*
      const int IN  = 512;
      const int OUT = IN * 16;

      uint8_t arr[OUT];
      memset(arr, 0, OUT);
      for (int j = 0; j < IN; j++)
      {
         Botan::SHA_256 sha256;
         uint8_t hashBin[32];
         auto hashRaw = sha256.process(std::to_string(j));
         memcpy(hashBin, hashRaw, 32);

         auto dst = Utils::arrayToUInt32(hashBin);
         arr[dst % OUT]++;
      }

      for (int j = 0; j < OUT; j++)
         if ((int)arr[j] > 1)
             std::cout << (int)arr[j] << ",";
      std::cout << std::endl;
*/
}



// *********************** PRIVATE METHODS: *********************



bool ClientProtocols::connectToResolver()
{
   try
   {
      std::cout << "Starting client functionality..." << std::endl;

      //connect over Tor to remote resolver
      remoteResolver_ = std::make_shared<SocksClient>("localhost", 9050);
      remoteResolver_->connectTo("129.123.7.39", 14321);
   }
   catch (boost::system::system_error const& ex)
   {
      std::cerr << ex.what() << std::endl;
      std::cerr << "Tor does not appear to be running! Aborting." << std::endl;
      return false;
   }

   return true;
}



std::pair<int, int> ClientProtocols::establishIPC()
{
   //const auto PIPE_CHECK = std::chrono::milliseconds(500);
   auto env = Environment::get();
   const auto QUERY_PATH    = env->getQueryPipe();
   const auto RESPONSE_PATH = env->getResponsePipe();

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
   std::string response = domain;
   while (Utils::strEndsWith(response, ".tor"))
   {
      std::cout << "Sending \"" << response << "\" to resolver..." << std::endl;
      response = remoteResolver_->sendReceive(response);
      std::cout << "Resolved to \"" << response << "\"" << std::endl;
   }
   return response;
}
