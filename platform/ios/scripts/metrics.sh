#!/usr/bin/env bash

set -e
set -o pipefail

# Generate stripped versions for every architecture
xcrun bitcode_strip build/ios/pkg/dynamic/Mapbox.framework/Mapbox -r -o  build/ios/pkg/dynamic/Mapbox-stripped
lipo build/ios/pkg/dynamic/Mapbox-stripped -extract armv7 -output build/ios/pkg/dynamic/Mapbox-stripped-armv7
lipo build/ios/pkg/dynamic/Mapbox-stripped -extract arm64 -output build/ios/pkg/dynamic/Mapbox-stripped-arm64
lipo build/ios/pkg/dynamic/Mapbox-stripped -extract x86_64 -output build/ios/pkg/dynamic/Mapbox-stripped-x86_64

# Track individual architectures
scripts/log_binary_size.sh "build/ios/pkg/dynamic/Mapbox-stripped-armv7"   "Platform=iOS,Arch=armv7"
scripts/log_binary_size.sh "build/ios/pkg/dynamic/Mapbox-stripped-arm64"   "Platform=iOS,Arch=arm64"
scripts/log_binary_size.sh "build/ios/pkg/dynamic/Mapbox-stripped-x86_64"  "Platform=iOS,Arch=x86_64"

# Track overall library size
scripts/log_binary_size.sh "build/ios/pkg/dynamic/Mapbox-stripped"         "Platform=iOS,Arch=Dynamic"
