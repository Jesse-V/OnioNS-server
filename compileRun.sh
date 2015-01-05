#!/bin/sh
cmake .
cpus=$(grep -c ^processor /proc/cpuinfo)

((cd libs/libscrypt-1.20/ && make -j $cpus))
if (make -j $cpus) then
    ./EsgalDNS
fi
