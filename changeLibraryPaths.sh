#!/bin/sh

cd Build
make clean
make
rm -rf TLD.app/Contents/Frameworks/*
cp /usr/X11/lib/libpng15.15.dylib TLD.app/Contents/Frameworks/
cp /usr/local/lib/liblinear.dylib TLD.app/Contents/Frameworks/
install_name_tool -change /usr/X11/lib/libpng15.15.dylib @executable_path/../Frameworks/libpng15.15.dylib TLD.app/Contents/MacOS/tld
install_name_tool -id @executable_path/../Frameworks/libpng15.15.dylib TLD.app/Contents/Frameworks/libpng15.15.dylib
install_name_tool -change /usr/local/lib/liblinear.dylib @executable_path/../Frameworks/liblinear.dylib TLD.app/Contents/MacOS/tld
#install_name_tool -id @executable_path/../Frameworks/liblinear.dylib TLD.app/Contents/Frameworks/liblinear.dylib

exit

