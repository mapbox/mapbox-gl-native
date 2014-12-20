#!/usr/bin/env bash

set -e
set -o pipefail

NAME=$1


export CC=clang CXX=clang++
export CXXFLAGS="-Wno-unknown-warning-option -Wno-unused-local-typedefs -Wno-unknown-pragmas ${CXXFLAGS}";

export MASON_PLATFORM=android

mkdir ./android/java/app/src/main/res/raw/
echo "${ANDROID_KEY}" >> ./android/java/app/src/main/res/raw/token.txt

make android BUILDTYPE=$BUILDTYPE

aws s3 cp ./android/java/app/build/outputs/apk/app-debug.apk s3://mapbox-gl-testing/android/${NAME}/app-debug.apk
aws s3 cp ./android/java/app/build/outputs/apk/app-release-unsigned.apk s3://mapbox-gl-testing/android/${NAME}/app-release-unsigned.apk
