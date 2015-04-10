
#ifndef FLAGS_HPP
#define FLAGS_HPP

#include <memory>
#include <string>

class Flags
{
   public:
      static std::shared_ptr<Flags> get();

      enum OperationMode
      {
         CLIENT, SERVER
      };

      bool parse(int argc, char** argv);
      OperationMode getMode();
      bool verbosityEnabled();

   private:
      static std::shared_ptr<Flags> singleton_;

      OperationMode mode_;
      bool verbosity_;
};

#endif
