#!/bin/bash

set -e

# Inspect binary.
if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    ldd ./lib/mapbox-gl-native.node
else
    otool -L ./lib/mapbox-gl-native.node
fi

COMMIT_MESSAGE=$(git show -s --format=%B $TRAVIS_COMMIT | tr -d '\n')

if test "${COMMIT_MESSAGE#*'[publish binary]'}" != "$COMMIT_MESSAGE" && [[ $CXX == "clang++" ]]; then
    npm install aws-sdk

    ./node_modules/.bin/node-pre-gyp package

    if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
        ./node_modules/.bin/node-pre-gyp testpackage
    fi

    ./node_modules/.bin/node-pre-gyp publish info

    if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
        rm -rf build
        rm -rf lib
        npm install --fallback-to-build=false
        npm test
    fi
fi
