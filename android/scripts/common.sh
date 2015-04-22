#!/usr/bin/env bash

set -e
set -o pipefail

NAME=$1

case `uname -s` in
    'Darwin') export JOBS=`sysctl -n hw.ncpu` ;;
    'Linux')  export JOBS=`nproc` ;;
    *)        export JOBS=1 ;;
esac


export CC=clang CXX=clang++
export MASON_PLATFORM=android

mkdir -p ./android/java/MapboxGLAndroidSDKTestApp/src/main/res/raw/
echo "${MAPBOX_ACCESS_TOKEN}" >> ./android/java/MapboxGLAndroidSDKTestApp/src/main/res/raw/token.txt

make android -j$JOBS BUILDTYPE=$BUILDTYPE JOBS=$JOBS

S3_PREFIX=s3://mapbox/mapbox-gl-native/android/build/${NAME}
APK_OUTPUTS=./android/java/MapboxGLAndroidSDKTestApp/build/outputs/apk

# Upload the shared object.
aws s3 cp \
    ./build/android-${ANDROID_ABI}/${BUILDTYPE}/lib.target/libmapbox-gl.so \
    ${S3_PREFIX}/libmapbox-gl.so

# Upload either the debug or the release build
if [ ${BUILDTYPE} == "Debug" ] ; then
    aws s3 cp \
        ${APK_OUTPUTS}/MapboxGLAndroidSDKTestApp-debug.apk \
        ${S3_PREFIX}/MapboxGLAndroidSDKTestApp-debug.apk
elif [ ${BUILDTYPE} == "Release" ] ; then
    aws s3 cp \
        ${APK_OUTPUTS}/MapboxGLAndroidSDKTestApp-release-unsigned.apk \
        ${S3_PREFIX}/MapboxGLAndroidSDKTestApp-release-unsigned.apk
fi
