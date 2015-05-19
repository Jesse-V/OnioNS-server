
#include "ClientProtocols.hpp"
#include "../common/records/CreateR.hpp"
#include "../common/Environment.hpp"
#include "../common/utils.hpp"
#include <botan/base64.h>
#include <sys/stat.h>
#include <thread>
#include <fstream>
#include <iostream>


void ClientProtocols::listenForDomains()
{
/*   std::fstream cacheFile("/var/lib/tor-onions/cache.txt");
   if (!cacheFile)
      throw std::runtime_error("Cannot open Record cache!");
   std::string str((std::istreambuf_iterator<char>(cacheFile)),
      std::istreambuf_iterator<char>());




   auto record = parseRecord(str);
*/
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

   for (int j = 0; j < 600; j++) //finite resolving
   {
      //read .tor domain from Tor Browser
      ssize_t readLength = read(queryPipe, (void*)buffer, MAX_LEN);
      if (readLength < 0)
      {
         //std::cerr << "Read error from IPC named pipe!" << std::endl;
      }
      else if (readLength > 0)
      {
         std::string domainIn(buffer, static_cast<ulong>(readLength - 1));
         std::cout << "Read \"" << domainIn << "\" from Tor." << std::endl;

         std::string onionOut = resolve(domainIn);

         std::cout << "Writing \"" << onionOut << "\" to Tor... ";
         ssize_t ret = write(responsePipe, onionOut.c_str(), onionOut.length() + 1);
         std::cout << "done, " << ret << std::endl;
      }

      //delay before polling pipe again
      std::this_thread::sleep_for(POLL_DELAY);
   }

   std::cout << "Closing down resolution loop. Cleanup." << std::endl;

   //tear down file descriptors
   close(queryPipe);
   close(responsePipe);
}



std::string ClientProtocols::resolve(const std::string& torDomain)
{
   try
   {
      std::string domain = torDomain;

      while (Utils::strEndsWith(domain, ".tor"))
      {
         //check cache first
         auto iterator = cache_.find(domain);
         if (iterator == cache_.end())
         {
            std::cout << "Sending \"" << domain << "\" to resolver..." << std::endl;
            auto response = remoteResolver_->sendReceive(domain + "\r\n");
            std::cout << "Received Record response." << std::endl;

            auto dest = getDestination(parseRecord(response), domain);

            //if (response == "<MALFORMED>")
            //   return "";

            cache_[domain] = dest;
            domain = dest;
         }
         else
         {
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            domain = iterator->second; //retrieve from cache
         }
      }

      if (!Utils::strEndsWith(domain, ".onion"))
         throw std::runtime_error("\"" + domain + "\" is not a HS address!");
      return domain;
   }
   catch (std::runtime_error& re)
   {
      std::cerr << "Err: " << re.what() << std::endl;
   }

   return "<OnioNS_READFAIL>";
}



std::shared_ptr<Record> ClientProtocols::parseRecord(const std::string& json)
{
   Json::Value rVal = toJSON(json);

   auto cHash     = rVal["cHash"].asString();
   auto contact   = rVal["contact"].asString();
   auto nonce     = rVal["nonce"].asString();
   auto pow       = rVal["pow"].asString();
   auto pubHSKey  = rVal["pubHSKey"].asString();
   auto recordSig = rVal["recordSig"].asString();
   auto timestamp = rVal["timestamp"].asString();
   auto type      = rVal["type"].asString();

   if (type != "Create")
      throw std::invalid_argument("Record parsing: not a Create Record!");

   NameList nameList;
   Json::Value list = rVal["nameList"];
   auto sources = list.getMemberNames();
   for (auto source : sources)
      nameList.push_back(std::make_pair(source, list[source].asString()));

   //std::cout << "done." << std::endl;

   //std::cout << "Decoding into Record... ";
   auto key = base64ToRSA(pubHSKey);
   auto createR = std::make_shared<CreateR>(cHash, contact, nameList, nonce,
      pow, recordSig, key, timestamp);
   //std::cout << "done." << std::endl;

   std::cout << "Checking validity... ";
   std::cout.flush();
   bool tmp = false;
   createR->computeValidity(&tmp);
   std::cout << "done." << std::endl;

   if (createR->hasValidSignature())
      std::cout << "Record signature is valid." << std::endl;
   else
      throw std::runtime_error("Bad signature on Record!");

   if (createR->isValid())
      std::cout << "Record proof-of-work is valid." << std::endl;
   else
      throw std::runtime_error("Record is not valid!");

   std::cout << "Record check complete." << std::endl;

   return createR;
}



Json::Value ClientProtocols::toJSON(const std::string& json)
{
   //std::cout << "Parsing JSON... ";

   Json::Value rVal;
   Json::Reader reader;

   if (!reader.parse(json, rVal))
      throw std::invalid_argument("Failed to parse Record!");

   if (!rVal.isMember("nameList"))
      throw std::invalid_argument("Record parsing: missing NameList");

   return rVal;
}



std::string ClientProtocols::getDestination(const std::shared_ptr<Record>& record,
   const std::string& source)
{
   NameList list = record->getNameList();
   for (auto pair : list)
      if (pair.first == source)
         return pair.second;

   throw std::runtime_error("Record does not contain \"" + source + "\"!");
}



// ***************************** PRIVATE METHODS *****************************



Botan::RSA_PublicKey* ClientProtocols::base64ToRSA(const std::string& base64)
{
   //decode public key
   unsigned long expectedSize = Utils::decode64Estimation(base64.length());
   uint8_t* keyBuffer = new uint8_t[expectedSize];
   size_t len = Botan::base64_decode(keyBuffer, base64, false);

   //interpret and parse into public RSA key
   std::istringstream iss(std::string(reinterpret_cast<char*>(keyBuffer), len));
   Botan::DataSource_Stream keyStream(iss);
   return dynamic_cast<Botan::RSA_PublicKey*>(Botan::X509::load_key(keyStream));
}



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
