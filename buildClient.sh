#!/bin/sh

cpus=$(grep -c ^processor /proc/cpuinfo)

cd tor-client
./configure
make -j $cpus

cp src/or/tor ../

# http://linux.byexamples.com/archives/163/how-to-create-patch-file-using-patch-and-diff/
