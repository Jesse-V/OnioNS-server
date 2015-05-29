
#include "Mirror.hpp"
#include "tcp/Server.hpp"
#include "containers/Cache.hpp"
#include "../common/Common.hpp"
#include <fstream>
#include <iostream>


void Mirror::startServer()
{
  // todo: load Records from the cache file, add to Cache

  std::cout << "Loading Record cache... " << std::endl;
  std::fstream cacheFile("/var/lib/tor-onions/cache.txt");
  if (!cacheFile)
    throw std::runtime_error("Cannot open cache!");

  // parse cache file into JSON object
  Json::Value cacheValue;
  Json::Reader reader;
  std::string json((std::istreambuf_iterator<char>(cacheFile)),
                   std::istreambuf_iterator<char>());
  if (!reader.parse(json, cacheValue))
  {
    std::cerr << "Failed to parse cache!" << std::endl;
    return;
  }

  // interpret JSON as Records and load into cache
  std::cout << "Preparing Records... " << std::endl;
  std::vector<RecordPtr> records;
  for (int n = 0; n < cacheValue.size(); n++)
    records.push_back(Common::get().parseRecord(cacheValue[n]));
  Cache::get().add(records);

  Server s(Environment::SERVER_PORT);
  s.start();
}
