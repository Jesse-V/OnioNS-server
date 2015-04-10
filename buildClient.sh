#!/bin/sh

cpus=$(grep -c ^processor /proc/cpuinfo)

cd tor-client-src

if [ ! -f "Makefile" ]; then
   ./configure
fi

make -j $cpus

cp src/or/tor ../tor-client

# http://linux.byexamples.com/archives/163/how-to-create-patch-file-using-patch-and-diff/
