#!/bin/sh

cpus=$(grep -c ^processor /proc/cpuinfo)

if [ ! -d "tor-client-orig" ]; then
    echo "Creating backup!"
    cp -r tor-client tor-client-orig    #create backup
fi

cd tor-client
./configure
make -j $cpus

cp src/or/tor ../

# http://linux.byexamples.com/archives/163/how-to-create-patch-file-using-patch-and-diff/
