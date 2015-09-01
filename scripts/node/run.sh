#!/usr/bin/env bash

set -e
set -o pipefail

source ./scripts/${TRAVIS_OS_NAME}/setup.sh

BUILDTYPE=${BUILDTYPE:-Release}

################################################################################
# Build
################################################################################

source ~/.nvm/nvm.sh
nvm use $NODE_VERSION

mapbox_time "checkout_styles" \
git submodule update --init styles

mapbox_time "compile_program" \
npm install --build-from-source

################################################################################
# Test
################################################################################

# Travis OS X has no GPU
if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    mapbox_time "checkout_test_suite" \
    git submodule update --init test/suite

    mapbox_time "run_tests" \
    npm test

    mapbox_time "run_render_tests" \
    npm run test-suite

    if [ ! -z "${AWS_ACCESS_KEY_ID}" ] && [ ! -z "${AWS_SECRET_ACCESS_KEY}" ] ; then
      # Install and add awscli to PATH for uploading the results
        mapbox_time "install_awscli" \
        pip install --user awscli
        export PATH="`python -m site --user-base`/bin:${PATH}"

        pushd test/suite
        mapbox_time "deploy_results" \
        ./bin/deploy_results.sh
        popd
    fi
fi
