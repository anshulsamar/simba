#!/bin/sh

# visualtld.sh
# ------------
# Usage: sh visualtld filePath startingFrame
# Example: sh visualtld ~/testVideo 5
# Takes a directory of images and using the tldDrawer program outputs a video with a TLD tracker.

#Error checking

if [ "$#" -ne 2 ]
then
    echo "Usage: sh visualtld.sh filePath startingFrame"
    exit 1
fi

#Copies directory of images to tmp
mkdir -v tmp
#cp -vr "$1/." tmp
cd ./tmp
x=1
y=1

#Creates symbolic links for images from starting point (arg2)
for i in "$1/"*jpg 
do
    if [ "$x" -lt $2 ]
    then
	x=$(($x+1))
	continue
    fi
    counter=$(printf %d $y) 
    ln -s "$i" ./image"$counter".jpg
    y=$(($y+1))
done
exit
#Create video and apply tld
ffmpeg -f image2 -i ./image%d.jpg ../video.mpg
cd ../
./tld video.mpg 334 205 366 242 #> tldOutput.txt
rm -rf video.mpg

#Use TLD data to create images with "tracking rectangles"
#./tldDrawer tldOutput.txt
#cd tmp

#Create video
#ffmpeg -f image2 -i ./image%d.jpg ../video.mpg
#cd ../
#rm -rf tmp
#rm -rf tldOutput.txt
exit