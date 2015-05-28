#!/bin/sh

echo "Preparing assets..."
cp -rl debian src # this cannot be a symlink
gzip --best -c debian/extra_includes/manpage > src/debian/extra_includes/onions.1.gz

echo "Preparing build..."
mkdir -p build/
cd build
cmake ../src # -DCMAKE_BUILD_TYPE=Debug

echo "Compiling..."
cpus=$(grep -c ^processor /proc/cpuinfo)
make -j $cpus
