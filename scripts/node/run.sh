#!/usr/bin/env bash

set -e
set -o pipefail

source ./scripts/${TRAVIS_OS_NAME}/setup.sh

BUILDTYPE=${BUILDTYPE:-Release}

################################################################################
# Build
################################################################################

mapbox_time "checkout_styles" \
git submodule update --init styles

mapbox_time "compile_program" \
pushd platform/node && npm install --build-from-source && popd

################################################################################
# Test
################################################################################

mapbox_time "checkout_test_suite" \
git submodule update --init test/suite

mapbox_time "run_tests" \
pushd platform/node && npm test && popd

mapbox_time "run_render_tests" \
pushd platform/node && npm run test-suite && popd
