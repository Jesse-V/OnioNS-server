
#include "Mirror.hpp"
#include <onions-common/Log.hpp>
#include <onions-common/Utils.hpp>
#include <botan/botan.h>
#include <popt.h>

// Botan::LibraryInitializer init("thread_safe");

int main(int argc, char** argv)
{
  // do not rearrange or compact these declarations or strange popt errors occur
  bool quorumNode = false;
  char* logPath = NULL;
  bool license = false;

  struct poptOption po[] = {
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
      {"quorum",
       'q',
       POPT_ARG_NONE,
       &quorumNode,
       0,
       "Causes the server to run as an authoritative Quorum server. The "
       "default is to run as a normal name server.",
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

  Mirror::startServer(quorumNode);

  return EXIT_SUCCESS;
}
