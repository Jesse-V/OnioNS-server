#!/bin/sh

# dependencies: dput

./clean.sh

name="tor-onions_0.1.0.3" #major.minor.patch.build

tar -czf ${name}.orig.tar.gz src/ #http://xkcd.com/1168/
echo "Tarball creation step complete."

#http://www.cyberciti.biz/faq/linux-unix-creating-a-manpage/
myManPath="debian/extra_includes"
gzip --best -c ${myManPath}/manpage > ${myManPath}/OnioNS.1.gz
echo "Manpage creation step complete."

cd src/
cp -rl ../debian debian # this cannot be a symlink
dpkg-buildpackage -S -sa -kAD97364FC20BEC80
cd ..
echo "Debian packaging step complete."

dput ppa:jvictors/testing ${name}_source.changes

rm -rf src/debian/
rm -f ${name}.orig.tar.gz ${name}.debian.tar.gz
rm -f ${name}.dsc ${name}.dsc ${name}_source.changes
rm -f ${name}_source.ppa.upload
rm -f debian/extra_includes/OnioNS.1.gz
echo "Cleanup complete."

echo "Done pushing packages. Check email for Launchpad build reports."
