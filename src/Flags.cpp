
#include "Flags.hpp"
#include <libs/tclap/CmdLine.h>
#include <stdexcept>


bool Flags::parse(int argc, char** argv)
{
   if (argc <= 2)
      return false;

   std::string mode(argv[1]);
   if (mode == "client")
      mode_ = OperationMode::CLIENT;
   else if (mode == "server")
      mode_ = OperationMode::SERVER;
   else if (mode == "hs")
      mode_ = OperationMode::HIDDEN_SERVICE;
   else
   {
      std::cerr << "Unknown mode of operation! Exiting." << std::endl;
      std::cout << "Command not in the form \"tor-onions <mode> <flags>\" \n";
      return false;
   }

   TCLAP::SwitchArg verboseFlag("v", "verbose",
      "Verbose printing to stdout.", false);

   TCLAP::SwitchArg licenseFlag("l", "license",
      "Prints license information and exits.", false);

   TCLAP::CmdLine cmd(R".(Examples:
      tor-onions client
      tor-onions server
      ).", '=', "<unknown>");

   cmd.add(verboseFlag);
   cmd.add(licenseFlag);

   cmd.parse(argc - 1, argv + 1);

   if (licenseFlag.isSet())
   {
      std::cout << "Modified/New BSD License" << std::endl;
      return false;
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
