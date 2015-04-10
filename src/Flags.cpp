
#include "Flags.hpp"
#include <libs/tclap/CmdLine.h>
#include <stdexcept>


std::shared_ptr<Flags> Flags::singleton_ = 0;
std::shared_ptr<Flags> Flags::get()
{
   if (singleton_)
      return singleton_;

   singleton_ = std::make_shared<Flags>();
   return singleton_;
}



bool Flags::parse(int argc, char** argv)
{
   TCLAP::ValueArg<std::string> modeFlag("m", "mode",
      "Specifies the mode, a local client or a remote server.", false,
      "blah blah", "mode");

   TCLAP::SwitchArg verboseFlag("v", "verbose",
      "Verbose printing to stdout.", false);

   TCLAP::SwitchArg licenseFlag("l", "license",
      "Prints license information and exits.", false);

   TCLAP::CmdLine cmd(R".(Examples:
      tor-onions --mode=client
      tor-onions --mode=server
      ).", '=', "<unknown>");

   cmd.add(modeFlag);
   cmd.add(verboseFlag);
   cmd.add(licenseFlag);

   cmd.parse(argc, argv);

   if (licenseFlag.isSet())
   {
      std::cout << "Modified/New BSD License" << std::endl;
      return false;
   }

   if (modeFlag.isSet())
   {
      if (modeFlag.getValue() == "client")
         mode_ = OperationMode::CLIENT;
      else if (modeFlag.getValue() == "server")
         mode_ = OperationMode::SERVER;
      else
      {
         std::cerr << "Unknown mode! Exiting." << std::endl;
         return false;
      }
   }

   verbosity_ = verboseFlag.isSet();

   return true;
}



Flags::OperationMode Flags::getMode()
{
   return mode_;
}



bool Flags::verbosityEnabled()
{
   return verbosity_;
}
