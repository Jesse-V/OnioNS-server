#!/bin/sh

#remove OnioNS build files
rm -rf build/

#remove libscrypt build files
((cd src/libs/libscrypt-1.20/ && make clean))

#clean up built binaries
rm tor OnioNS_d

cd tor-client
make distclean

echo "Successfully removed the build directory."
