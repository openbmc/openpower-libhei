#!/bin/sh

# Verify input.
if [ ! -d "$1" ]; then
    echo "Invalid directory: $1" 1>&2
    exit 1
fi

# Simply list out all of the JSON files in the given directory.
for i in "$1"/*.json; do
    if [ -f "$i" ]; then
        echo "$i"
    fi
done
