#!/bin/sh

# This script performs two static analysis checks of src, first using Clang's
# scan-build program and the second using cppcheck. As scan-build integrates
# nicely into the build process, the code is also compiled. However, due to the
# extra analysis, this script can take some time to complete so it's best run
# infrequently, such as before a release.

# Please install clang-3.8 and cppcheck before running this script.
# If your distribution has clang-analyzer-3.8, you may need to install that as well.

# in some distributions the path is /usr/share/clang/scan-build-3.8/c++-analyzer
export CXX=/usr/share/clang/scan-build-3.8/libexec/c++-analyzer
export CC=/usr/share/clang/scan-build-3.8/libexec/ccc-analyzer

# delete any previous build as linking fails if there's a mix of compilers
rm -rf build src/libs/libjson-rpc-cpp/build

mkdir -p build/
cd build
scan-build-3.8 cmake ../src -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_C_COMPILER=$CC
# -DCMAKE_BUILD_TYPE=Debug

# 2>&1 | grep -F -v "src/libs"

echo "Compiling with Clang static analysis...  -------------------------------"
rm -rf /tmp/scan-build-*
if (scan-build-3.8 -maxloop 16 -enable-checker core -enable-checker cplusplus -disable-checker deadcode -enable-checker security -enable-checker unix make -j $(grep -c ^processor /proc/cpuinfo)) then

  if [ $(ls /tmp/scan-build-* | wc -l 2> /dev/null) -gt 0 ]; then
    echo "Failure: static analysis contains reports."
    cd ..
    rm -rf build/

  else
    echo "Additional static analysis...        ----------------------------------------------"
    cd ..

    find src -type f -follow -print | grep -F -v "src/libs" | grep -E "\.h|\.c" | cppcheck -i "src/spec/rpc_spec.json" --enable=all --platform=unix64 --inconclusive --file-list=-

    echo "Success: compilation and scan-build check successful!"
  fi
else
  echo "Failure: compilation errors."

  cd ..
  rm -rf build/
fi
