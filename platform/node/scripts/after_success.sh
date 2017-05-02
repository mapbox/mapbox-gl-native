#!/bin/bash

set -e
set -o pipefail

if [[ -n ${PUBLISH:-} ]]; then
    if [[ "${BUILDTYPE}" == "Release" ]]; then
        ./node_modules/.bin/node-pre-gyp package publish info
    else
        ./node_modules/.bin/node-pre-gyp package publish info --debug
    fi
fi
