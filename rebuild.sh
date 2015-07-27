#!/bin/sh

echo "Re-styling code...        ----------------------------------------------"

# make the code-style consistent
for f in $(find src/ -type f -name "*.c*" | grep -v "libs"); do
   clang-format-3.6 -style="{BasedOnStyle: chromium, BreakBeforeBraces: Allman, MaxEmptyLinesToKeep: 3}" -i $f
done
# AlignConsecutiveAssignments: true
for f in $(find src/ -type f -name "*.h*" | grep -v "libs"); do
   clang-format-3.6 -style="{BasedOnStyle: chromium, BreakBeforeBraces: Allman, MaxEmptyLinesToKeep: 3}" -i $f
done

echo "Creating manpage...       ----------------------------------------------"

gzip --best -c src/assets/manpage > src/assets/onions-server.1.gz

echo "Preparing build...        ----------------------------------------------"

export CXX=/usr/bin/clang++
export CC=/usr/bin/clang

mkdir -p build/
cd build
cmake ../src # -DCMAKE_BUILD_TYPE=Debug

echo "Compiling...              ----------------------------------------------"
make -j $(grep -c ^processor /proc/cpuinfo)

echo "Static analysis...        ----------------------------------------------"

cd ..
cppcheck --enable=all --platform=unix64 --inconclusive src/*
