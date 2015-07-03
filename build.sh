#!/bin/sh

echo "Preparing assets..."
cp -rl debian src # this cannot be a symlink

echo "Preparing build..."
mkdir -p build/
cd build
cmake ../src # -DCMAKE_BUILD_TYPE=Debug

echo "Compiling..."
cpus=$(grep -c ^processor /proc/cpuinfo)
make -j $cpus
