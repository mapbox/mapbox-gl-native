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
make node -j${JOBS} BUILDTYPE=${BUILDTYPE} 

################################################################################
# Test
################################################################################

mapbox_time "checkout_test_suite" \
git submodule update --init test/suite

mapbox_time "run_tests" \
npm test

mapbox_time "run_render_tests" \
npm run test-suite
