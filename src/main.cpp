
#include "Server.hpp"
#include <onions-common/Log.hpp>
#include <onions-common/Constants.hpp>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include <botan/botan.h>
#include <argtable2.h>
#include <iostream>


Botan::LibraryInitializer init("thread_safe");
void parseFlags(int, char**, arg_str*, arg_file*, arg_int*, arg_int*, arg_lit*);
void startServer(short, arg_str*, short, bool);


int main(int argc, char** argv)
{
  // http://argtable.sourceforge.net/doc/argtable2.html,
  // http://argtable.sourceforge.net/example/myprog.c
  struct arg_str* bindIP =
      arg_str0("a", "address", "<IPv4>",
               "TCP IPv4 address to bind to, default 127.9.0.53");
  struct arg_file* logFile =
      arg_file0("o", "output", "<path>", "The filepath for event logging");
  struct arg_int* socksPort =
      arg_int0("p", "port", "<integer>", "Tor's SOCKS5 port, default 9050");
  struct arg_int* controlPort =
      arg_int0("p", "control", "<integer>", "Tor's control port, default 9051");
  struct arg_lit* qNode = arg_lit0(
      "q", "quorum", "Runs the server as an authoritative Quorum server");

  parseFlags(argc, argv, bindIP, logFile, socksPort, controlPort, qNode);
  startServer(socksPort->ival[0], bindIP, controlPort->ival[0],
              qNode->count > 0);

  /*
    jsonrpc::HttpServer httpd(Const::SERVER_PORT, bindIP->sval[0]);
    Server s(httpd, socksPort->ival[0], qNode->count > 0);
    s.start();

    // arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return EXIT_SUCCESS;
    */
  /*
    NameList list;
    list.push_back(std::make_pair("a", "example2.tor"));
    auto r = std::make_shared<CreateR>(
        Utils::loadKey(
            "/home/jesse/Git_Projects/OnioNS/OnioNS-server/private_key"),
        "example.tor", "CA80");
    r->setSubdomains(list);

    std::cout << "**" << std::endl;
    using namespace std::chrono;
    auto start = steady_clock::now();
    r->makeValid(1);
    auto diff = duration_cast<milliseconds>(steady_clock::now() -
    start).count();
    std::cout << (diff / 1000.0f) << std::endl;
    */
}


void parseFlags(int argc,
                char** argv,
                arg_str* bindIP,
                arg_file* logFile,
                arg_int* socksPort,
                arg_int* controlPort,
                arg_lit* qNode)
{
  // list all arguments that can be resolved instantly
  struct arg_lit* license =
      arg_lit0("L", "license", "Prints software license and exits.");
  struct arg_lit* help = arg_lit0("h", "help", "Print this help and exit.");
  struct arg_end* end = arg_end(20);
  void* argtable[] = {bindIP, logFile, socksPort, controlPort,
                      qNode,  license, help,      end};
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
  controlPort->ival[0] = 9051;
  // logFile->filename = new char[1][];
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
    std::cerr << "Parsing error. Try '" << name
              << " --help' for more information." << std::endl;
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



// blocking method
void startServer(short socksPort,
                 arg_str* bindIP,
                 short controlPort,
                 bool asQuorumNode)
{
  jsonrpc::HttpServer httpd(Const::SERVER_PORT, bindIP->sval[0]);
  Server s(httpd, socksPort, controlPort, asQuorumNode);

  s.StartListening();

  // wait for quit character.
  // I don't expect for the user to send this, so this is essentially blocking
  Log::get().notice("Server is online, listening for connections...");
  char c = 'c';
  while (c != 'q')
    c = getchar();

  Log::get().notice("Shutting down the server...");
  s.StopListening();
}
