#!/usr/bin/env bash

set -e
set -o pipefail

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    #
    # build & test Linux
    #
    make linux -j4 BUILDTYPE=${BUILDTYPE}
    make test -j4 BUILDTYPE=${BUILDTYPE}
    ./scripts/run_tests.sh
    (cd ./node_modules/mapbox-gl-test-suite/ && (./bin/compare_images.js || true))

    if [ ! -z "${AWS_ACCESS_KEY_ID}" ] && [ ! -z "${AWS_SECRET_ACCESS_KEY}" ] ; then
        (cd ./node_modules/mapbox-gl-test-suite/ && ./bin/deploy_results.sh)
    fi

elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    #
    # build OS X
    #
    make xproj-cli
    xcodebuild -project ./build/macosx/mapboxgl-app.xcodeproj -jobs 4
    #
    # build iOS
    #
    git submodule init
    git submodule update
    make iproj-cli
    xcodebuild -project ./build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj -sdk iphonesimulator ONLY_ACTIVE_ARCH=NO -jobs 4
fi
