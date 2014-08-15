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
    # setup code signing per http://docs.travis-ci.com/user/common-build-problems/#Mac%3A-Code-Signing-Errors
    #
    KEY_CHAIN=ios-build.keychain
    security create-keychain -p travis $KEY_CHAIN
    security default-keychain -s $KEY_CHAIN
    security unlock-keychain -p travis $KEY_CHAIN
    security set-keychain-settings -t 3600 -u $KEY_CHAIN
    #
    # build OS X
    #
    make xproj-cli
    xcodebuild clean -project ./build/macosx/mapboxgl-app.xcodeproj
    xcodebuild -project ./build/macosx/mapboxgl-app.xcodeproj
    #
    # build iOS
    #
    git clone --depth 1 https://github.com/mapbox/mapbox-gl-cocoa ios/mapbox-gl-cocoa
    make iproj-cli
    xcodebuild clean -project ./build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj
    xcodebuild -project ./build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj
fi
