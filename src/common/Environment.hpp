
#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <memory>
#include <string>

class Environment
{
   public:
      static Environment& get()
      {
         static Environment instance;
         return instance;
      }

      void setQueryPipe(const std::string& pipePath);
      std::string getQueryPipe();

      void setResponsePipe(const std::string& pipePath);
      std::string getResponsePipe();

   private:
      Environment() {}
      Environment(Environment const&) = delete;
      void operator=(Environment const&) = delete;
      static std::shared_ptr<Environment> singleton_;

      std::string queryPath_, responsePath_;
};

#endif
