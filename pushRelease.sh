#!/bin/sh

version="0.2.2.3" #major.minor.patch.build
edition="trusty"  #[trusty | utopic | vivid | wily]
name="tor-onions_"$version"~"$edition

tar -czf ${name}.orig.tar.gz src/ #http://xkcd.com/1168/
echo "Tarball creation step complete."

#http://www.cyberciti.biz/faq/linux-unix-creating-a-manpage/
cp -rl debian src/debian # this cannot be a symlink
gzip --best -c debian/extra_includes/manpage > src/debian/extra_includes/onions.1.gz

cd src/
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

echo ""
echo "Github release commands:"
echo "git tag -s -u AD97364FC20BEC80 v"$version" -m 'Release "$version"'"
echo "git push origin --tags"
echo "Packages @ https://launchpad.net/~jvictors/+archive/ubuntu/tor-dev/+packages"
echo ""

echo "Done pushing packages. Check email for Launchpad build reports."
