#!/bin/sh

#remove OnioNS build files
rm -rf build/

#remove libscrypt build files
((cd src/libs/libscrypt-1.20/ && make clean))

#clean up built binaries
rm tor OnioNS_d

# if tor-client backup exists, restore from it
if [ -d "tor-client-orig" ]; then
    rm -rf tor-client
    mv tor-client-orig tor-client
fi

echo "Successfully removed the build directory."
