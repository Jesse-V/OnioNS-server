#!/bin/sh

cpus=$(grep -c ^processor /proc/cpuinfo)

cp -r tor-client tor-client-orig

cd tor-client
./configure
make -j $cpus

cp src/or/tor ../

