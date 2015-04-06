
#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <string>

class Environment
{
   public:
      static Environment& get();

      void setReadPipe(const std::string& pipePath);
      std::string getReadPipe();

   private:
      static Environment* singleton_;
};

#endif
