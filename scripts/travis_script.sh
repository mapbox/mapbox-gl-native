#!/usr/bin/env bash

set -e
set -o pipefail

mapbox_time "checkout_styles" \
git submodule update --init styles

if [[ $MASON_PLATFORM == "android" ]]; then
    #mapbox_time "compile_program" \
    #make android BUILDTYPE=${BUILDTYPE}
    ./android/scripts/run-build.sh

    #if [[ $TESTMUNK == "yes" ]]; then
        #mapbox_time_start "upload_testmunk"
        #(cd ./android/test/ && ./upload_testmunk.sh ../java/app/build/outputs/apk/app-debug.apk)
        #mapbox_time_finish
    #fi

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
    ./scripts/run_tests.sh

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
    mapbox_time "create_osx_project" \
    make build/macosx/mapboxgl-app.xcodeproj

    mapbox_time "build_osx" \
    xcodebuild -project ./build/macosx/mapboxgl-app.xcodeproj -jobs $JOBS

    #
    # build iOS
    #
    mapbox_time "checkout_cocoa_bindings" \
    git submodule update --init ios/mapbox-gl-cocoa

    mapbox_time "create_ios_project" \
    make build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj

    mapbox_time "build_ios" \
    xcodebuild -project ./build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj -sdk iphonesimulator ONLY_ACTIVE_ARCH=NO -jobs $JOBS
fi
