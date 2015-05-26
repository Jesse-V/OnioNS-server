

#include "Flags.hpp"
#include "common/Environment.hpp"
#include "client/Client.hpp"
#include "hs/HS.hpp"
#include "mirror/Mirror.hpp"
#include <botan/botan.h>
#include <fstream>
#include <iostream>

Botan::LibraryInitializer init("thread_safe");

int main(int argc, char** argv)
{
  if (!Flags::get().parse(argc, argv))
    return EXIT_SUCCESS;

  if (!Flags::get().verbosityEnabled())
  {  // https://bbs.archlinux.org/viewtopic.php?id=79378
    // https://groups.google.com/forum/#!topic/comp.lang.c++.moderated/ggl_2Ii3aVM
    // http://www.velocityreviews.com/forums/showpost.php?p=1501892&postcount=2
    std::ofstream nullOut("/dev/null");
    std::cout.rdbuf(nullOut.rdbuf());
  }

  if (Flags::get().getMode() == Flags::OperationMode::CLIENT)
  {
    Client::get().listenForDomains();
  }
  else if (Flags::get().getMode() == Flags::OperationMode::HIDDEN_SERVICE)
  {
    if (Flags::get().getCommand() == Flags::Command::CREATE_RECORD)
      HS::get().createRecord();
  }
  else if (Flags::get().getMode() == Flags::OperationMode::MIRROR)
  {
    Mirror::get().startServer();
  }

  return EXIT_SUCCESS;
}
