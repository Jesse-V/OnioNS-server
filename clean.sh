#!/bin/sh

#remove OnioNS build files
rm -rf build/

#remove libscrypt build files
((cd src/libs/libscrypt-1.20/ && make clean))

#clean up built binaries
rm tor-client OnioNS_d

cd tor-client-src
make distclean
rm -rf autom4te.cache

echo "Successfully removed the build directory."
