#!/bin/sh

#remove OnioNS build files
rm -rf build/ src/debian

#remove libscrypt build files
((cd src/libs/libscrypt && make clean))

echo "Successfully removed the build directory."
