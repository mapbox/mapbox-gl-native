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
    (cd ./node_modules/mapbox-gl-test-suite/ && (./bin/compare_images.js || true; ./bin/deploy_results.sh))
elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    #
    # build OS X
    #
    make xproj-cli
    xcodebuild -project ./build/macosx/mapboxgl-app.xcodeproj
    #
    # build iOS
    #
    git clone --depth 1 https://github.com/mapbox/mapbox-gl-cocoa ios/mapbox-gl-cocoa
    make iproj-cli
    xcodebuild -project ./build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj -sdk iphonesimulator ONLY_ACTIVE_ARCH=NO
fi
