
#include "Server.hpp"
#include <onions-common/Log.hpp>
#include <onions-common/Constants.hpp>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include <botan/botan.h>
#include <argtable2.h>
#include <iostream>

Botan::LibraryInitializer init("thread_safe");


void parseOptions(int argc, char** argv, arg_str* bindIP, arg_file* logFile, arg_int* socksPort, arg_lit* qNode)
{
  //list all arguments that can be resolved instantly
  struct arg_lit* license = arg_lit0("L", "license", "Prints software license and exits.");
  struct arg_lit* help = arg_lit0("h", "help", "Print this help and exit.");
  struct arg_end* end = arg_end(20);
  void* argtable[] = {bindIP, logFile, socksPort, qNode, license, help, end};
  const char* name = "onions-server";

  // verify that the argtable was allocated sucessfully
  if (arg_nullcheck(argtable) != 0)
  {
    std::cerr << "Insufficient memory to allocate argtable." << std::endl;
    exit(EXIT_FAILURE);
  }

  // set default values for the arguments
  bindIP->sval[0] = "127.9.0.53";
  socksPort->ival[0] = 9050;
  //logFile->filename = new char[1][];
  logFile->filename[0] = "-";

  int nerrors = arg_parse(argc, argv, argtable);

  // print usage
  if (help->count > 0)
  {
    std::cout << "Usage: " << name;
    arg_print_syntax(stdout, argtable, "\n");
    arg_print_glossary(stdout, argtable, "  %-25s %s\n");
    exit(EXIT_SUCCESS);
  }
  
  // check and display any errors
  if (nerrors > 0)
  {
    arg_print_errors(stdout, end, name);
    std::cerr << "Parsing error. Try '" << name << " --help' for more information." << std::endl;
    exit(EXIT_FAILURE);
  }
  
  if (license->count > 0)
  {
    std::cout << "Modified/New BSD License" << std::endl;
    exit(EXIT_SUCCESS);
  }
  
  if (logFile->count > 0 && std::string(logFile->filename[0]) != "-")
    Log::setLogPath(std::string(logFile->filename[0]));
}



int main(int argc, char** argv)
{
  // http://argtable.sourceforge.net/doc/argtable2.html, http://argtable.sourceforge.net/example/myprog.c
  struct arg_str* bindIP = arg_str0("a", "address", "<IPv4>", "TCP IPv4 address to bind to, default 127.9.0.53");
  struct arg_file* logFile = arg_file0("o", "output", "<path>", "The filepath for event logging");
  struct arg_int* socksPort = arg_int0("p", "port", "<integer>", "Tor's SOCKS5 port, default 9050");
  struct arg_lit* qNode = arg_lit0("q", "quorum", "Runs the server as an authoritative Quorum server");
  
  parseOptions(argc, argv, bindIP, logFile, socksPort, qNode);
  
  jsonrpc::HttpServer httpd(Const::SERVER_PORT, bindIP->sval[0]);
  Server s(httpd, socksPort->ival[0], qNode->count > 0);
  s.start();
  
  //arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
  return EXIT_SUCCESS;
}

