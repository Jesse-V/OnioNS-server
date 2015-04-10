
#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <memory>
#include <string>

class Environment
{
   public:
      static std::shared_ptr<Environment> get();

      void setQueryPipe(const std::string& pipePath);
      std::string getQueryPipe();

      void setResponsePipe(const std::string& pipePath);
      std::string getResponsePipe();

   private:
      static std::shared_ptr<Environment> singleton_;
      std::string queryPath_, responsePath_;
};

#endif
