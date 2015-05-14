#!/usr/bin/env bash

set -e
set -o pipefail
set -u

xcodebuild \
    -sdk iphoneos \
    -project ./build/ios/gyp/ios.xcodeproj \
    -configuration Debug \
    -target All \
    analyze | xcpretty | grep -q 'The following commands produced analyzer issues'
