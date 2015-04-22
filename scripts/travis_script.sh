#!/usr/bin/env bash

COMMIT_MESSAGE=$(git show -s --format=%B $1 | tr -d '\n')
PUBLISH_TAG=$(echo "$COMMIT_MESSAGE" | grep -oE '\[publish [a-z0-9\.\-]+\]' | grep -oE '[a-z0-9\.\-]+' | tail -n1)
PUBLISH_PLATFORM=$(echo "$PUBLISH_TAG" | awk -F '-v' '{ print $1 }')
PUBLISH_VERSION=$(echo "$PUBLISH_TAG" | awk -F '-v' '{ print $2 }')

set -e
set -o pipefail

mapbox_time "checkout_styles" \
git submodule update --init styles

if [[ $MASON_PLATFORM == "android" ]]; then
    ./android/scripts/run-build.sh

elif [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    #
    # build & test Linux
    #
    mapbox_time "compile_program" \
    make linux -j$JOBS BUILDTYPE=${BUILDTYPE}

    mapbox_time "compile_tests" \
    make test -j$JOBS BUILDTYPE=${BUILDTYPE}

    mapbox_time "checkout_test_suite" \
    git submodule update --init test/suite

    mapbox_time "run_tests" \
    make test-* BUILDTYPE=${BUILDTYPE}

    mapbox_time "compare_results" \
    ./scripts/compare_images.sh

    if [ ! -z "${AWS_ACCESS_KEY_ID}" ] && [ ! -z "${AWS_SECRET_ACCESS_KEY}" ] ; then
        mapbox_time_start "deploy_results"
        (cd ./test/suite/ && ./bin/deploy_results.sh)
        mapbox_time_finish
    fi

elif [[ ${TRAVIS_OS_NAME} == "osx" && ${MASON_PLATFORM} == "osx" ]]; then
    #
    # build OS X
    #
    mapbox_time "build_osx" \
    make xosx -j$JOBS

elif [[ ${TRAVIS_OS_NAME} == "osx" && ${MASON_PLATFORM} == "ios" ]]; then
    #
    # build & package iOS
    #
    mapbox_time "package_ios"
    make ipackage
    #
    # conditionally deploy iOS build
    #
    if [[ -n "$PUBLISH_TAG" ]]; then
        mapbox_time "deploy_ios"
        ./scripts/publish_ios.sh "$PUBLISH_VERSION"
    fi
fi
