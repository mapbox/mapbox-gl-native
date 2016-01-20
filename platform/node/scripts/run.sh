#!/usr/bin/env bash

set -e
set -o pipefail

source ./platform/${TRAVIS_OS_NAME}/scripts/setup.sh

BUILDTYPE=${BUILDTYPE:-Release}

################################################################################
# Build
################################################################################

source ~/.nvm/nvm.sh
nvm use $NODE_VERSION
npm install --build-from-source

################################################################################
# Test
################################################################################

# https://github.com/mapbox/mapbox-gl-native/issues/2150
if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    npm test
    npm run test-suite
fi
