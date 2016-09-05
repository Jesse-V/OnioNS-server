#!/bin/sh

# This script re-styles the code to a format very similar to Chromium's style
# and then builds the project using the Clang 3.8 compiler. This is the main
# script to use if you are compiling frequently.

# Please install clang-3.8 and clang-format-3.8 before running this script.

echo "Re-styling code...        ----------------------------------------------"

# make the code-style consistent
for f in $(find src/ -type f -name "*.c*" | grep -v "libs"); do
   clang-format-3.8 -style="{BasedOnStyle: chromium, BreakBeforeBraces: Allman, MaxEmptyLinesToKeep: 3}" -i $f
done
# AlignConsecutiveAssignments: true
for f in $(find src/ -type f -name "*.h*" | grep -v "libs"); do
   clang-format-3.8 -style="{BasedOnStyle: chromium, BreakBeforeBraces: Allman, MaxEmptyLinesToKeep: 3}" -i $f
done

echo "Preparing build...        ----------------------------------------------"

export CXX=clang++-3.8
export CC=clang-3.8

# set up build
mkdir -p build
cd build
cmake ../src -DCMAKE_BUILD_TYPE=Debug

echo "Compiling...              ----------------------------------------------"
if (make -j $(grep -c ^processor /proc/cpuinfo)) then
  cd ..
  echo "Compilation successful!"
else
  cd ..
  rm -rf build/
fi
