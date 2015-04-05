#!/bin/sh

#remove OnioNS build files
rm -rf build/

#remove libscrypt build files
((cd src/libs/libscrypt-1.20/ && make clean))

#clean up tor-client
mv -f tor-client-orig tor-client
rm tor

echo "Successfully removed the build directory."
