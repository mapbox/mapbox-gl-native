#!/usr/bin/env bash

set -e
set -o pipefail

BUILDTYPE=${BUILDTYPE:-Release}

################################################################################
# Build
################################################################################

mapbox_time "checkout_styles" \
git submodule update --init styles

mapbox_time "compile_program" \
make xosx -j${JOBS} BUILDTYPE=${BUILDTYPE}

mapbox_time "compile_tests" \
make xtest -j${JOBS} BUILDTYPE=${BUILDTYPE}
