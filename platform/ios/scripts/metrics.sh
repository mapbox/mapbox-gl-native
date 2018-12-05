#!/usr/bin/env bash

set -e
set -o pipefail

# Generate stripped versions for every architecture
xcrun bitcode_strip build/ios/pkg/dynamic/Mapbox.framework/Mapbox -r -o build/ios/pkg/dynamic/Mapbox-stripped
strip -Sx build/ios/pkg/dynamic/Mapbox-stripped
lipo build/ios/pkg/dynamic/Mapbox-stripped -extract armv7 -output build/ios/pkg/dynamic/Mapbox-stripped-armv7
lipo build/ios/pkg/dynamic/Mapbox-stripped -extract arm64 -output build/ios/pkg/dynamic/Mapbox-stripped-arm64
lipo build/ios/pkg/dynamic/Mapbox-stripped -extract x86_64 -output build/ios/pkg/dynamic/Mapbox-stripped-x86_64

# Track individual architectures
scripts/check_binary_size.js "build/ios/pkg/dynamic/Mapbox-stripped-armv7"   "iOS armv7"
scripts/check_binary_size.js "build/ios/pkg/dynamic/Mapbox-stripped-arm64"   "iOS arm64"
scripts/check_binary_size.js "build/ios/pkg/dynamic/Mapbox-stripped-x86_64"  "iOS x86_64"

# Track overall library size
scripts/check_binary_size.js "build/ios/pkg/dynamic/Mapbox-stripped"         "iOS Dynamic"

if [[ $CIRCLE_BRANCH == master ]]; then
  # Build source data for http://mapbox.github.io/mapbox-gl-native/metrics/binary-size/
  # and log binary sizes to metrics warehouse
  scripts/publish_binary_size.js
fi

