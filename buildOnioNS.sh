#!/bin/sh

mkdir -p build/
cd build
cmake ../src # -DCMAKE_BUILD_TYPE=Debug

cpus=$(grep -c ^processor /proc/cpuinfo)
make -j $cpus
cp tor-onions* ../
