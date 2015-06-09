
#include "Flags.hpp"
#include "common/utils.hpp"
#include <tclap/CmdLine.h>
#include <stdexcept>


bool Flags::parse(int argc, char** argv)
{
  if (argc <= 1)
    return false;

  TCLAP::CmdLine cmd(R".(Examples:
      onions --client -v
      onions --hs -r --domain=<domain> --hskey=<keypath> -v
      ).",
                     '=', "<unknown>");

  TCLAP::SwitchArg licenseFlag("l", "license",
                               "Prints license information and exits.", false);

  TCLAP::SwitchArg clientMode("c", "client", "Switch to client mode.", false);
  TCLAP::SwitchArg mirrorMode("s", "server",
                              "Switch to name-server (Mirror) mode.", false);
  TCLAP::SwitchArg hsMode("d", "hs", "Switch to hidden service mode.", false);

  TCLAP::SwitchArg createRecord("r", "register", "Register a domain name.",
                                false);

  TCLAP::ValueArg<std::string> keyPath(
      "k", "hskey", "The path to the private hidden service RSA key.", false,
      "/var/lib/tor-onions/example.key", "keypath");
  TCLAP::ValueArg<std::string> mirrorIPVal(
      "m", "mirror", "The IPv4 address of the Mirror name server.", false,
      "129.123.7.8", "IPv4 addr");

  cmd.add(licenseFlag);

  cmd.add(clientMode);
  cmd.add(mirrorMode);
  cmd.add(hsMode);

  cmd.add(createRecord);
  cmd.add(keyPath);
  cmd.add(mirrorIPVal);

  cmd.parse(argc, argv);

  if (licenseFlag.isSet())
  {
    std::cout << "Modified/New BSD License" << std::endl;
    return false;
  }

  if (clientMode.isSet())
  {
    mode_ = OperationMode::CLIENT;
    if (!mirrorIPVal.isSet())
    {
      std::cerr << "Client mode, but missing Mirror IP!" << std::endl;
      return false;
    }

    mirrorIP_ = mirrorIPVal.getValue();
  }
  else if (mirrorMode.isSet())
    mode_ = OperationMode::MIRROR;
  else if (hsMode.isSet())
  {
    mode_ = OperationMode::HIDDEN_SERVICE;

    if (!keyPath.isSet())
    {
      std::cerr << "HS mode, but missing path to key! Specify with --hskey\n";
      return false;
    }
    // todo: check for --register
  }
  else
  {
    std::cerr << "No mode specified! Missing --client, --server, or --hs flags."
              << std::endl;
    return false;
  }

  if (createRecord.isSet())
    command_ = Command::CREATE_RECORD;

  keyPath_ = keyPath.getValue();

  return true;
}



Flags::OperationMode Flags::getMode() const
{
  return mode_;
}



Flags::Command Flags::getCommand() const
{
  return command_;
}



std::string Flags::getKeyPath() const
{
  return keyPath_;
}



std::string Flags::getMirrorIP() const
{
  return mirrorIP_;
}
