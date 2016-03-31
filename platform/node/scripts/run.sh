#!/usr/bin/env bash

set -e
set -o pipefail

source ./platform/${TRAVIS_OS_NAME}/scripts/setup.sh

BUILDTYPE=${BUILDTYPE:-Release}

source ~/.nvm/nvm.sh
nvm use $NODE_VERSION
npm install --build-from-source

npm test
npm run test-suite
