
#include "Mirror.hpp"
#include <onions-common/Log.hpp>
#include <onions-common/Utils.hpp>
#include <botan/botan.h>
#include <popt.h>

// Botan::LibraryInitializer init("thread_safe");

int main(int argc, char** argv)
{
  // do not rearrange or compact these declarations or strange popt errors occur
  bool authority = false;
  char* logPath = NULL;
  char* address = const_cast<char*>("0.0.0.0");
  bool license = false;
  ushort port = 10053;

  struct poptOption po[] = {
      {"address",
       'a',
       POPT_ARG_STRING,
       &address,
       0,
       "Specifies a TCP IPv4 address to bind to.",
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
       "Specifies the filepath for event logging.",
       "<path>",
      },
      {"port",
       'p',
       POPT_ARG_SHORT,
       &port,
       0,
       "TCP port to bind the server to on all interfaces.",
       "<port>"},
      {"quorum",
       'q',
       POPT_ARG_NONE,
       &authority,
       0,
       "Causes the server to run as an authoritative server. The default is "
       "to run as a normal name server.",
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

  Mirror::startServer(std::string(address), port, authority);

  return EXIT_SUCCESS;
}
