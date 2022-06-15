#!/bin/sh 
cd script
OUTPUT="$(java PlatformCheck)"
cd ..
if [ "${OUTPUT}" -eq "64" ] 
then
cp lib/native/libftd2xx_32.so.1.3.6 /usr/lib/libftd2xx.so.1.3.6
else
cp lib/native/libftd2xx_64.so.1.3.6 /usr/lib/libftd2xx.so.1.3.6
fi
ln -s /usr/lib/libftd2xx.so.1.3.6 /usr/lib/libftd2xx.so
ldconfig
