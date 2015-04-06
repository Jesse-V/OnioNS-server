
#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <memory>
#include <string>

class Environment
{
   public:
      static std::shared_ptr<Environment> get();

      void setReadPipe(const std::string& pipePath);
      std::string getReadPipe();

      void setWritePipe(const std::string& pipePath);
      std::string getWritePipe();

   private:
      static std::shared_ptr<Environment> singleton_;
      std::string readPipe_, writePipe_;
};

#endif
