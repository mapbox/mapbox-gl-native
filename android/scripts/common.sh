#!/usr/bin/env bash

set -e
set -o pipefail

NAME=$1
export JOBS=`nproc`

export CC=clang CXX=clang++
export MASON_PLATFORM=android

mkdir ./android/java/MapboxGLAndroidSDKTestApp/src/main/res/raw/
echo "${MAPBOX_ACCESS_TOKEN}" >> ./android/java/MapboxGLAndroidSDKTestApp/src/main/res/raw/token.txt

make android -j$JOBS BUILDTYPE=$BUILDTYPE JOBS=$JOBS

aws s3 cp ./build/android-${ANDROID_ABI}/${BUILDTYPE}/lib.target/libmapbox-gl.so s3://mapbox-gl-testing/android/${NAME}/libmapbox-gl.so
aws s3 cp ./android/java/MapboxGLAndroidSDKTestApp/build/outputs/apk/MapboxGLAndroidSDKTestApp-debug.apk s3://mapbox-gl-testing/android/${NAME}/MapboxGLAndroidSDKTestApp-debug.apk
aws s3 cp ./android/java/MapboxGLAndroidSDKTestApp/build/outputs/apk/MapboxGLAndroidSDKTestApp-release-unsigned.apk s3://mapbox-gl-testing/android/${NAME}/MapboxGLAndroidSDKTestApp-release-unsigned.apk
