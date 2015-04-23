#!/bin/bash

#
# Bash shell script to automate multiple runs of ./program2
#

DEBUG=0 # 1 to turn debug on

FRAME_LIMIT=100 # default max number of frames

if [ "$#" -eq 5 ]; then
    FRAME_LIMIT=$4
        
    # create files, or wipe their contents if they exist
    echo -n "" > "$2"
    
    # base runs with frames in [1,10]
    for (( nFrames=1; nFrames<=10; nFrames++ ))
    do
	if [ $DEBUG -eq 1 ]; then
	    echo "run ./program2 with $nFrames frames"
	fi
	echo "========== $nFrames frames ==========" >> "$2"
	./program2 "$nFrames" "$1" >> "$2"
    done

    # run with frames in [<frames-start>,FRAME_LIMIT]
    for (( nFrames="$3"; nFrames<="$FRAME_LIMIT"; nFrames+="$5" ))
    do
	if [ $DEBUG -eq 1 ]; then
	    echo "run ./program2 with $nFrames frames"
	fi
	echo "========== $nFrames frames ==========" >> "$2"
	./program2 "$nFrames" "$1" >> "$2"
    done

else
    echo "usage: $0 <input-file> <output-file> <frames-start> <frames-limit> <step-size>"
    exit 1
fi