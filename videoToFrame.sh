#!/bin/sh

command -v ffmpeg >/dev/null 2>&1 || { echo >&2 "I require ffmpeg but it's not installed.  Aborting."; exit 1; }

if [ "$#" -ne 5 ]
then
    echo "Usage: sh visualtld.sh videoPath imageDirectory requestedWidth requestedHeight framesPerSecond"
    echo "Requirements: Install ffmpeg in order to extract frames from videos."
    exit 1
fi

mkdir -v $2/tmp
ffmpeg -i $1 -r $5 -f image2 $2/tmp/%07d.png
ffmpeg -i $2/tmp/%07d.png -vf scale=$3:$4 $2/%07d.png
rm -rf $2/tmp

echo "Extracted frames and resized."


exit

