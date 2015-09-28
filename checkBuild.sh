#!/bin/sh

# This script performs two static analysis checks of src, first using Clang's
# scan-build program and the second using cppcheck. As scan-build integrates
# nicely into the build process, the code is also compiled. However, due to the
# extra analysis, this script can take some time to complete so it's best run
# infrequently, such as before a release.

# Please install clang-3.6 and cppcheck before running this script.

export CCC_CXX=clang++-3.6
export CCC_CC=clang-3.6

mkdir -p build/
cd build
scan-build-3.6 cmake ../src -DCMAKE_CXX_COMPILER=/usr/share/clang/scan-build-3.6/c++-analyzer -DCMAKE_C_COMPILER=/usr/share/clang/scan-build-3.6/ccc-analyzer # -DCMAKE_BUILD_TYPE=Debug

echo "Compiling with Clang static analysis...  -------------------------------"
rm -rf /tmp/scan-build-*
if (scan-build-3.6 -maxloop 16 -enable-checker core -enable-checker cplusplus -disable-checker deadcode -enable-checker security -enable-checker unix make -j $(grep -c ^processor /proc/cpuinfo)) then

  if [ $(ls /tmp/scan-build-* | wc -l 2> /dev/null) -gt 0 ]; then
    echo "Failure: static analysis contains reports."
    cd ..
    rm -rf build/

  else
    echo "Additional static analysis...        ----------------------------------------------"
    cd ..
    cppcheck --enable=all --platform=unix64 --inconclusive src/*

    echo "Success: compilation and scan-build check successful!"
  fi
else
  echo "Failure: compilation errors."

  cd ..
  rm -rf build/
fi
