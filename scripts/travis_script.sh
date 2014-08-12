#!/usr/bin/env bash

set -e
set -o pipefail

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    #
    # build & test Linux
    #
    make linux -j4 BUILDTYPE=${BUILDTYPE}
    make test -j4 BUILDTYPE=${BUILDTYPE}
    cd build/Testing
    for TEST in ./test_* ; do
        ${TEST}
    done
    (cd ./node_modules/mapbox-gl-test-suite/ && (./bin/compare_images.js; ./bin/deploy_results.sh))
elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    #
    # build OS X
    #
    make xproj-cli
    xcodebuild clean -project ./build/macosx/mapboxgl-app.xcodeproj | xcpretty -c
    xcodebuild -project ./build/macosx/mapboxgl-app.xcodeproj | xcpretty -c
    #
    # build iOS
    #
    git clone --depth 1 https://github.com/mapbox/mapbox-gl-cocoa ios/mapbox-gl-cocoa
    make iproj-cli
    xcodebuild clean -project ./build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj | xcpretty -c
    xcodebuild ./build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj | xcpretty -c
fi
