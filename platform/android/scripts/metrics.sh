#!/usr/bin/env bash

set -e
set -o pipefail

# Track individual architectures
scripts/check_binary_size.js "platform/android/MapboxGLAndroidSDK/build/intermediates/intermediate-jars/release/jni/armeabi-v7a/libmapbox-gl.so" "Android arm-v7"
scripts/check_binary_size.js "platform/android/MapboxGLAndroidSDK/build/intermediates/intermediate-jars/release/jni/arm64-v8a/libmapbox-gl.so"   "Android arm-v8"
scripts/check_binary_size.js "platform/android/MapboxGLAndroidSDK/build/intermediates/intermediate-jars/release/jni/x86/libmapbox-gl.so"         "Android x86"
scripts/check_binary_size.js "platform/android/MapboxGLAndroidSDK/build/intermediates/intermediate-jars/release/jni/x86_64/libmapbox-gl.so"      "Android x86_64"

# Track overall library size
scripts/check_binary_size.js "platform/android/MapboxGLAndroidSDK/build/outputs/aar/MapboxGLAndroidSDK-release.aar" "Android AAR"

if [[ $CIRCLE_BRANCH == master ]]; then
    scripts/publish_binary_size.js
fi
