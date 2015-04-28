#!/bin/sh

#remove OnioNS build files
rm -rf build/

#remove libscrypt build files
((cd src/libs/libscrypt && make clean))

#clean up built binaries
rm tor-client tor-onions

cd tor-client-src
make distclean
rm -rf autom4te.cache

echo "Successfully removed the build directory."
