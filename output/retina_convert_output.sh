#!/bin/zsh
ffmpeg -i "$1" -vf scale=iw/2:ih/2 "$2"