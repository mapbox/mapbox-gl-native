#!/usr/bin/env bash

set -e
set -o pipefail
set -u

# Disable static analyzer in third-party libraries with known issues:
# https://github.com/memononen/libtess2/pull/19
# https://sourceforge.net/p/polyclipping/bugs/134/
perl -pi -e 's/(isa = PBXBuildFile; fileRef = [0-9A-F]+ \/\* (clipper\.cpp|mesh\.c) \*\/;) }/$1 settings = {COMPILER_FLAGS = "-Qunused-arguments -Xanalyzer -analyzer-disable-all-checks"; }; }/g' \
    ./build/ios/mbgl.xcodeproj/project.pbxproj

xcodebuild \
    -sdk iphoneos \
    -project ./build/ios/gyp/ios.xcodeproj \
    -configuration Debug \
    -target All \
    analyze | xcpretty | tee ./build/ios/analyze.txt

! grep -q 'The following commands produced analyzer issues' ./build/ios/analyze.txt
