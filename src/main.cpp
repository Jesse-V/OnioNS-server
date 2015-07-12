
#include "Mirror.hpp"
#include <onions-common/Log.hpp>
#include <onions-common/Utils.hpp>
#include <botan/botan.h>
#include <popt.h>

Botan::LibraryInitializer init("thread_safe");

int main(int argc, char** argv)
{
  char* logPath = NULL;
  bool license = false, authority = false;
  short port = 10053;

  struct poptOption po[] = {
      {"authority",
       'a',
       POPT_ARG_NONE,
       &authority,
       11001,
       "Causes the server to run as an authoritative server. The default is "
       "to run as a normal name server."},
      {
       "output",
       'o',
       POPT_ARG_STRING,
       &logPath,
       11002,
       "Specifies the filepath for event logging.",
       "<path>",
      },
      {"port",
       'p',
       POPT_ARG_SHORT,
       &port,
       11003,
       "TCP port to bind the server to on all interfaces.",
       "<port>"},
      {
       "license",
       'L',
       POPT_ARG_NONE,
       &license,
       11004,
       "Prints software license and exit.",
      },
      POPT_AUTOHELP{NULL}};

  bool b = Utils::parse(
      argc, poptGetContext(NULL, argc, const_cast<const char**>(argv), po, 0));

  if (license)
  {
    std::cout << "Modified/New BSD License" << std::endl;
    return EXIT_SUCCESS;
  }

  if (logPath && std::string(logPath) != "-")
    Log::setLogPath(std::string(logPath));

  Mirror::startServer(port, authority);

  return EXIT_SUCCESS;
}
