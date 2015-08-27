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

# Travis OS X has no GPU
if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    mapbox_time "checkout_test_suite" \
    git submodule update --init test/suite

    mapbox_time "run_tests" \
    pushd platform/node && npm test && popd

    mapbox_time "run_render_tests" \
    pushd platform/node && npm run test-suite && popd

    if [ ! -z "${AWS_ACCESS_KEY_ID}" ] && [ ! -z "${AWS_SECRET_ACCESS_KEY}" ] ; then
      # Install and add awscli to PATH for uploading the results
        mapbox_time "install_awscli" \
        pip install --user awscli
        export PATH="`python -m site --user-base`/bin:${PATH}"

        mapbox_time "deploy_results" \
        pushd test/suite && ./bin/deploy_results.sh && popd
    fi
fi
