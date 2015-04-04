#!/bin/sh

./clean.sh

name="onions_0.1.0.1" #major.minor.patch.build

tar -czf ${name}.orig.tar.gz src/ #http://xkcd.com/1168/
echo "Successfully made tarball."

#http://www.cyberciti.biz/faq/linux-unix-creating-a-manpage/
myManPath="debian/extra_includes"
gzip --best -c ${myManPath}/manpage > ${myManPath}/OnioNS.1.gz
echo "Successfully made manpage."

cd src/
ln -s ../debian debian
dpkg-buildpackage -S -sa -k80FAAD97364FC20BEC80
echo "Successfully made signed Debian package."

#cd ..
#dput ppa:jvictors/testing ${name}_source.changes

rm -f debian
cd ..
rm -f ${name}.orig.tar.gz ${name}.debian.tar.gz
#rm -f ${name}.dsc ${name}.dsc ${name}_source.changes
#rm -f ${name}_source.ppa.upload
#rm -f debian/extra_includes/OnioNS.1.gz
echo "Successfully cleaned package build files and folders."

#echo "Done pushing packages. Check email for Launchpad build reports."
