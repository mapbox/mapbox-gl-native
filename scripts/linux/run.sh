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

################################################################################
# Deploy
################################################################################

if [ ! -z "${AWS_ACCESS_KEY_ID}" ] && [ ! -z "${AWS_SECRET_ACCESS_KEY}" ] ; then
    # Install and add awscli to PATH for uploading the results
    mapbox_time "install_awscli" \
    pip install --user awscli
    export PATH="`python -m site --user-base`/bin:${PATH}"

    mapbox_time_start "deploy_results"
    (cd ./test/suite/ && ./bin/deploy_results.sh)
    mapbox_time_finish
fi
