#!/usr/bin/env bash

set -e
set -o pipefail

if [ ! $(command -v qmake 2> /dev/null) ]; then
    echo "No qmake found - missing Qt development files"
    exit 1
fi
