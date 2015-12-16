#!/usr/bin/env bash

set -e
set -o pipefail
set -u

xcodebuild \
    -project ./test/ios/ios-tests.xcodeproj \
    -scheme 'Mapbox GL Tests' \
    -sdk iphonesimulator \
    -destination 'platform=iOS Simulator,name=iPhone 6,OS=latest' \
    -destination 'platform=iOS Simulator,name=iPad 2,OS=latest' \
    test \
    | xcpretty
