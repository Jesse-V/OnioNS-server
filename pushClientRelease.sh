#!/bin/sh

# dependencies: dput

./clean.sh

name="tor-client_0.2.6.6~0.0.0.1" #TorMajor.TorMinor.TorPatch.TorBuild.ClientPatch

tar -czf ${name}.orig.tar.gz src/ #http://xkcd.com/1168/
echo "Tarball creation step complete."

cd tor-0.2.6.6-client/
cp -rl ../debian-client debian # this cannot be a symlink
dpkg-buildpackage -S -sa -kAD97364FC20BEC80
cd ..
echo "Debian packaging step complete."

#dput -l ppa:jvictors/testing ${name}_source.changes

rm -rf tor-0.2.6.6-client/debian/
rm -f ${name}.orig.tar.gz ${name}.debian.tar.gz
rm -f ${name}.dsc ${name}.dsc ${name}_source.changes
rm -f ${name}_source.ppa.upload
echo "Cleanup complete."

echo "Done pushing packages. Check email for Launchpad build reports."
