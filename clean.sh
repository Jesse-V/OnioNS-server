#!/bin/sh

#remove OnioNS build files
rm -rf build/

#remove libscrypt build files
((cd src/libs/libscrypt-1.20/ && make clean))

#clean up tor-client
rm -rf tor-client
mv tor-client-orig tor-client
rm tor OnioNS_d

echo "Successfully removed the build directory."
