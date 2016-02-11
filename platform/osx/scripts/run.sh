#!/usr/bin/env bash

set -e
set -o pipefail

source ./platform/osx/scripts/setup.sh

BUILDTYPE=${BUILDTYPE:-Release}

################################################################################
# Build
################################################################################

mapbox_time "compile_render_binary" \
make render -j${JOBS} BUILDTYPE=${BUILDTYPE}

mapbox_time "compile_tests" \
make xtest -j${JOBS} BUILDTYPE=${BUILDTYPE}
