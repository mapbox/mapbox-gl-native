#!/usr/bin/env bash

set -e
set -o pipefail

source ./scripts/linux/setup.sh

BUILDTYPE=${BUILDTYPE:-Release}

################################################################################
# Build
################################################################################

mapbox_time "checkout_styles" \
git submodule update --init styles

mapbox_time "compile_program" \
make linux -j${JOBS} BUILDTYPE=${BUILDTYPE}

mapbox_time "compile_render_binary" \
make render -j${JOBS} BUILDTYPE=${BUILDTYPE}

mapbox_time "compile_tests" \
make test -j${JOBS} BUILDTYPE=${BUILDTYPE}

################################################################################
# Test
################################################################################

mapbox_time "checkout_test_suite" \
git submodule update --init test/suite

mapbox_time "run_tests" \
make test-* BUILDTYPE=${BUILDTYPE}

mapbox_time "compare_results" \
./scripts/compare_images.sh
