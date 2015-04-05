#!/bin/sh

# dependencies: dput

./ClangBuildClient.sh
mv tor-client debian/extra_includes/tor-client #prep Tor for packaging

./clean.sh

name="tor-onions_0.1.3.5~trusty" #major.minor.patch.build~[trusty | utopic | vivid]

tar -czf ${name}.orig.tar.gz src/ #http://xkcd.com/1168/
echo "Tarball creation step complete."

#http://www.cyberciti.biz/faq/linux-unix-creating-a-manpage/
myManPath="debian/extra_includes"
gzip --best -c ${myManPath}/manpage > ${myManPath}/tor-onions.1.gz
echo "Manpage creation step complete."

cd src/
cp -rl ../debian debian # this cannot be a symlink
dpkg-buildpackage -S -sa -kAD97364FC20BEC80
cd ..
echo "Debian packaging step complete."

dput ppa:jvictors/testing ${name}_source.changes # -l

rm -rf src/debian/
rm -f ${name}.orig.tar.gz ${name}.debian.tar.gz
rm -f ${name}.dsc ${name}.dsc ${name}_source.changes
rm -f ${name}_source.ppa.upload
rm -f debian/extra_includes/tor-client
echo "Cleanup complete."

echo "Done pushing packages. Check email for Launchpad build reports."
