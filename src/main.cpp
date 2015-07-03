

#include "Flags.hpp"
#include "mirror/Mirror.hpp"
#include <botan/botan.h>

Botan::LibraryInitializer init("thread_safe");

int main(int argc, char** argv)
{
  if (!Flags::get().parse(argc, argv))
    return EXIT_FAILURE;

  Mirror::get().startServer();

  return EXIT_SUCCESS;
}
