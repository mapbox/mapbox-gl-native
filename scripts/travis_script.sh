#!/usr/bin/env bash

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

elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    #
    # build OS X
    #
    mapbox_time "build_osx_project" \
    make xosx -j$JOBS

    #
    # build iOS
    #
    mapbox_time "build_ios_project_device_release" \
    make ios -j$JOBS
    mapbox_time "build_ios_project_simulator_debug" \
    make isim -j$JOBS
fi
