
//#include "Mirror.hpp"
#include <onions-common/Log.hpp>
#include <onions-common/Utils.hpp>
#include <botan/botan.h>
#include <argtable2.h>
#include <iostream>

Botan::LibraryInitializer init("thread_safe");

int main(int argc, char** argv)
{
  // http://argtable.sourceforge.net/doc/argtable2.html, http://argtable.sourceforge.net/example/myprog.c
  struct arg_str* bindIP = arg_str0("a", "address", "<IPv4>", "TCP IPv4 address to bind to.");
  struct arg_lit* help    = arg_lit0("h", "help",                    "Print this help and exit");
  struct arg_lit* license = arg_lit0("L", "license", "Prints software license and exits.");
  struct arg_file* logFile = arg_file0("o", "output", "<path>", "The filepath for event logging.");
  struct arg_int* socksPort  = arg_int0("p", "port", "integer", "Tor's SOCKS5 port.");
  struct arg_lit* qNode = arg_lit0("q", "quorum", "Runs the server as an authoritative Quorum server.");
  struct arg_end  *end     = arg_end(20);
  void* argtable[] = {bindIP, help, license, logFile, socksPort, qNode, end};
  const char* progname = "myprog";
  int nerrors;
  int exitcode=0;

  /* verify the argtable[] entries were allocated sucessfully */
  if (arg_nullcheck(argtable) != 0)
  {
    /* NULL entries were detected, some allocations must have failed */
    printf("%s: insufficient memory\n",progname);
    exitcode=1;
    goto exit;
  }

  /* set any command line default values prior to parsing */
  bindIP->sval[0] = "127.53.53.53";
  socksPort->ival[0] = 9050;

  /* Parse the command line as defined by argtable[] */
  nerrors = arg_parse(argc,argv,argtable);

  /* special case: '--help' takes precedence over error reporting */
  if (help->count > 0)
  {
    printf("Usage: %s", progname);
    arg_print_syntax(stdout,argtable,"\n");
    printf("Sample output!");
    arg_print_glossary(stdout,argtable,"  %-25s %s\n");
    exitcode=0;
    goto exit;
  }

  /* If the parser returned any errors then display them and exit */
  if (nerrors > 0)
  {
    /* Display the error details contained in the arg_end struct.*/
    arg_print_errors(stdout,end,progname);
    printf("Try '%s --help' for more information.\n",progname);
    exitcode=1;
    goto exit;
  }

  /* special case: uname with no command line options induces brief help */
  if (argc==1)
  {
    printf("Try '%s --help' for more information.\n",progname);
    exitcode=0;
    goto exit;
  }

  /* normal case: take the command line options at face value */
  //exitcode = mymain(list->count, recurse->count, repeat->ival[0],
  //    defines->sval, defines->count,
  //    outfile->filename[0], verbose->count,
   //   infiles->filename, infiles->count);

  exit:
    /* deallocate each non-null entry in argtable[] */
    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));

  return exitcode;

/*
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
  */
  return EXIT_SUCCESS;
}

