#!/usr/bin/env bash

set -e
set -o pipefail

# Track individual architectures
scripts/log_binary_size.sh "platform/android/MapboxGLAndroidSDK/src/main/jniLibs/armeabi/libmapbox-gl.so"     "Platform=Android,Arch=arm-v5"
scripts/log_binary_size.sh "platform/android/MapboxGLAndroidSDK/src/main/jniLibs/armeabi-v7a/libmapbox-gl.so" "Platform=Android,Arch=arm-v7"
scripts/log_binary_size.sh "platform/android/MapboxGLAndroidSDK/src/main/jniLibs/arm64-v8a/libmapbox-gl.so"   "Platform=Android,Arch=arm-v8"
scripts/log_binary_size.sh "platform/android/MapboxGLAndroidSDK/src/main/jniLibs/x86/libmapbox-gl.so"         "Platform=Android,Arch=x86"
scripts/log_binary_size.sh "platform/android/MapboxGLAndroidSDK/src/main/jniLibs/x86_64/libmapbox-gl.so"      "Platform=Android,Arch=x86_64"
scripts/log_binary_size.sh "platform/android/MapboxGLAndroidSDK/src/main/jniLibs/mips/libmapbox-gl.so"        "Platform=Android,Arch=mips"

# Track overall library size
scripts/log_binary_size.sh "platform/android/MapboxGLAndroidSDK/build/outputs/aar/MapboxGLAndroidSDK-release.aar" "Platform=Android,Arch=Archive"
