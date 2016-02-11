#!/usr/bin/env bash

set -e
set -o pipefail

source ./platform/linux/scripts/setup.sh

BUILDTYPE=${BUILDTYPE:-Release}

################################################################################
# Build
################################################################################

mapbox_time "compile_program" \
make linux -j${JOBS} BUILDTYPE=${BUILDTYPE}

mapbox_time "compile_render_binary" \
make render -j${JOBS} BUILDTYPE=${BUILDTYPE}

mapbox_time "compile_offline_binary" \
make offline -j${JOBS} BUILDTYPE=${BUILDTYPE}

mapbox_time "compile_tests" \
make test -j${JOBS} BUILDTYPE=${BUILDTYPE}

################################################################################
# Test
################################################################################

mapbox_time "run_tests" \
make test-* BUILDTYPE=${BUILDTYPE}
