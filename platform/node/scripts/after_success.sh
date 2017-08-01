#!/bin/bash

set -e
set -o pipefail

if [[ "${PUBLISH:-}" == true ]]; then
    if [[ "${BUILDTYPE}" == "RelWithDebInfo" ]]; then
        ./node_modules/.bin/node-pre-gyp package publish info
    elif [[ "${BUILDTYPE}" == "Debug" ]]; then
        ./node_modules/.bin/node-pre-gyp package publish info --debug
    else
        echo "error: must provide either Debug or RelWithDebInfo for BUILDTYPE"
        exit 1
    fi
else
    echo "Not publishing. $PUBLISH $CIRCLE_TAG"
fi
