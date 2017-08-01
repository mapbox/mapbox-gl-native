#!/bin/bash

set -e
set -o pipefail

PACKAGE_JSON_VERSION=`node -e "console.log(require('./package.json').version)"`

echo $PACKAGE_JSON_VERSION
echo $CIRCLE_TAG

if [[ "${CIRCLE_TAG}" == "node-v${PACKAGE_JSON_VERSION}" ]]; then
    if [[ "${BUILDTYPE}" == "RelWithDebInfo" ]]; then
        ./node_modules/.bin/node-pre-gyp package publish info
    elif [[ "${BUILDTYPE}" == "Debug" ]]; then
        ./node_modules/.bin/node-pre-gyp package publish info --debug
    else
        echo "error: must provide either Debug or RelWithDebInfo for BUILDTYPE"
        exit 1
    fi
fi
