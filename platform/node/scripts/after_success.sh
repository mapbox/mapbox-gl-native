#!/bin/bash

set -e
set -o pipefail

if [[ -n ${PUBLISH:-} ]]; then
    if [[ "${BUILDTYPE}" == "Debug" ]]; then
        echo "Please run this script in release mode (BUILDTYPE=Release)."
        exit 1
    else
        ./node_modules/.bin/node-pre-gyp package publish info
    fi
fi
