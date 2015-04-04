#!/bin/sh

mkdir -p build/
cd build
cmake ../src -DCMAKE_BUILD_TYPE=Debug

cpus=$(grep -c ^processor /proc/cpuinfo)
#((cd ../src/libs/libscrypt-1.20/ && make -j $cpus))
make -j $cpus
