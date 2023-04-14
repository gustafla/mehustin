#!/bin/sh
cd $(dirname "$0")/bin
export LD_LIBRARY_PATH=.
./mehustin -w 1920 -h 1080 -f -b 120 -r 8 "$@" Mehu
