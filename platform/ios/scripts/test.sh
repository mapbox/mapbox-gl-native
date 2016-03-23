#!/usr/bin/env bash

set -e
set -o pipefail
set -u

xcodebuild \
    -project ./platform/ios/test/ios-tests.xcodeproj \
    -scheme 'Mapbox GL Tests' \
    -sdk iphonesimulator \
    -destination 'platform=iOS Simulator,name=iPhone 6,OS=latest' \
    test
