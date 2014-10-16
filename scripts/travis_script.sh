#!/usr/bin/env bash

set -e
set -o pipefail

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    #
    # build & test Linux
    #
    mapbox_time "compile program" \
    make linux -j4 BUILDTYPE=${BUILDTYPE}

    mapbox_time "install tests" \
    make test -j4 BUILDTYPE=${BUILDTYPE}

    mapbox_time "run tests" \
    ./scripts/run_tests.sh

    mapbox_time "compare results" \
    (cd ./node_modules/mapbox-gl-test-suite/ && (./bin/compare_images.js || true))

    if [ ! -z "${AWS_ACCESS_KEY_ID}" ] && [ ! -z "${AWS_SECRET_ACCESS_KEY}" ] ; then
        mapbox_time "deploy results" \
        (cd ./node_modules/mapbox-gl-test-suite/ && ./bin/deploy_results.sh)
    fi

elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    #
    # build OS X
    #
    mapbox_time "create osx project" \
    make xproj-cli

    mapbox_time "build osx" \
    xcodebuild -project ./build/macosx/mapboxgl-app.xcodeproj -jobs 4

    #
    # build iOS
    #
    git submodule init

    mapbox_time "load submodules" \
    git submodule update

    mapbox_time "create ios project" \
    make iproj-cli

    mapbox_time "build ios" \
    xcodebuild -project ./build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj -sdk iphonesimulator ONLY_ACTIVE_ARCH=NO -jobs 4
fi
