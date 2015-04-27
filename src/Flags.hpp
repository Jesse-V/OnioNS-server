
#ifndef FLAGS_HPP
#define FLAGS_HPP

#include <memory>
#include <string>

class Flags
{
   public:
      static Flags& get()
      {
         static Flags instance;
         return instance;
      }

      enum OperationMode
      {
         CLIENT, SERVER, HIDDEN_SERVICE
      };

      bool parse(int argc, char** argv);
      OperationMode getMode();
      bool verbosityEnabled();

   private:
      Flags() {}
      Flags(Flags const&) = delete;
      void operator=(Flags const&) = delete;
      static std::shared_ptr<Flags> singleton_;

      OperationMode mode_;
      bool verbosity_;
};

#endif
