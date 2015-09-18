
#include "Mirror.hpp"
#include <onions-common/Log.hpp>
#include <onions-common/Utils.hpp>
#include <botan/botan.h>
#include <popt.h>
#include <iostream>

// Botan::LibraryInitializer init("thread_safe");

int main(int argc, char** argv)
{
  // do not rearrange or compact these declarations or strange popt errors occur
  bool quorumNode = false;
  char* logPath = NULL;
  char* bindIP = const_cast<char*>("127.53.53.53");
  bool license = false;
  ushort socksPort = 9050;

  struct poptOption po[] = {
      {"address",
       'a',
       POPT_ARG_STRING,
       &bindIP,
       0,
       "A TCP IPv4 address to bind to.",
       "<address>"},
      {"license",
       'L',
       POPT_ARG_NONE,
       &license,
       0,
       "Prints software license and exit.",
       NULL},
      {
       "output",
       'o',
       POPT_ARG_STRING,
       &logPath,
       0,
       "The filepath for event logging.",
       "<path>",
      },
      {"port",
       'p',
       POPT_ARG_SHORT,
       &socksPort,
       0,
       "SOCKS5 port for anonymous communication.",
       "<port>"},
      {"quorum",
       'q',
       POPT_ARG_NONE,
       &quorumNode,
       0,
       "Runs the server as an authoritative Quorum server.",
       NULL},
      POPT_AUTOHELP{NULL, 0, 0, NULL, 0, NULL, NULL}};

  if (!Utils::parse(
          poptGetContext(NULL, argc, const_cast<const char**>(argv), po, 0)))
  {
    std::cout << "Failed to parse command-line arguments. Aborting.\n";
    return EXIT_FAILURE;
  }

  if (license)
  {
    std::cout << "Modified/New BSD License" << std::endl;
    return EXIT_SUCCESS;
  }

  if (logPath && std::string(logPath) != "-")
    Log::setLogPath(std::string(logPath));

  Mirror::get().startServer(std::string(bindIP), socksPort, quorumNode);

  return EXIT_SUCCESS;
}
