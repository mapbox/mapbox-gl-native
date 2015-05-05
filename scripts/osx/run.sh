#!/usr/bin/env bash

set -e
set -o pipefail

BUILDTYPE=${BUILDTYPE:-Release}

source ./scripts/travis_helper.sh

# Add Mason to PATH
export PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"

# Set the core file limit to unlimited so a core file is generated upon crash
ulimit -c unlimited -S

################################################################################
# Build
################################################################################

mapbox_time "checkout_styles" \
git submodule update --init styles

mapbox_time "compile_program" \
make xosx -j${JOBS} BUILDTYPE=${BUILDTYPE}

mapbox_time "compile_tests" \
make xtest -j${JOBS} BUILDTYPE=${BUILDTYPE}
