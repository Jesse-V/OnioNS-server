
#include "Mirror.hpp"
#include "tcp/Server.hpp"
#include "containers/Cache.hpp"
#include "../common/Common.hpp"
#include <fstream>
#include <iostream>


void Mirror::startServer()
{
  // todo: load Records from the cache file, add to Cache

  std::fstream cacheFile("/var/lib/tor-onions/cache.txt");
  if (!cacheFile)
    throw std::runtime_error("Cannot open cache!");

  std::string json((std::istreambuf_iterator<char>(cacheFile)),
                   std::istreambuf_iterator<char>());
  /*
    Json::Value cacheValue;
    Json::Reader reader;
    if (!reader.parse(json, cacheValue))
    {
      std::cerr << "Failed to parse cache!" << std::endl;
      return;
    }*/

  std::vector<RecordPtr> records;
  records.push_back(Common::get().parseRecord(json));
  Cache::get().add(records);

  Server s(Environment::SERVER_PORT);
  s.start();
}
