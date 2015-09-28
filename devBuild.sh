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

#date style to match Tor's manpage
date=$(date +%m\\/%d\\/%Y)
gzip --best -c src/assets/manpage | sed "s/<DATE>/$date/g" > src/assets/onions-server.1.gz

echo "Preparing build...        ----------------------------------------------"

export CXX=/usr/bin/clang++-3.6
export CC=/usr/bin/clang-3.6

mkdir -p build/
cd build
cmake ../src # -DCMAKE_BUILD_TYPE=Debug

echo "Compiling...              ----------------------------------------------"
if (make -j $(grep -c ^processor /proc/cpuinfo)) then
  cd ..
  echo "Compilation successful!"
else
  cd ..
  rm -rf build/
fi
