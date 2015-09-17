#!/usr/bin/env bash

set -e
set -o pipefail

# Add Mason to PATH
export PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"
export MASON_PLATFORM=android
export MASON_ANDROID_ABI=${ANDROID_ABI}

################################################################################
# Build
################################################################################

mapbox_time "checkout_styles" \
git submodule update --init styles

mkdir -p ./android/java/MapboxGLAndroidSDKTestApp/src/main/res/raw
echo "${MAPBOX_ACCESS_TOKEN}" > ./android/java/MapboxGLAndroidSDKTestApp/src/main/res/raw/token.txt

mapbox_time "compile_library" \
make android-lib HOST_VERSION=${ANDROID_ABI} -j${JOBS} BUILDTYPE=${BUILDTYPE}

mapbox_time "build_apk" \
make android HOST_VERSION=${ANDROID_ABI} -j${JOBS} BUILDTYPE=${BUILDTYPE}

################################################################################
# Deploy
################################################################################

if [ ! -z "${AWS_ACCESS_KEY_ID}" ] && [ ! -z "${AWS_SECRET_ACCESS_KEY}" ] ; then
    # Install and add awscli to PATH for uploading the results
    mapbox_time "install_awscli" \
    pip install --user awscli
    export PATH="`python -m site --user-base`/bin:${PATH}"

    mapbox_time_start "deploy_results"
    echo "Deploying results..."

    S3_PREFIX=s3://mapbox/mapbox-gl-native/android/build/${TRAVIS_JOB_NUMBER}
    APK_OUTPUTS=./android/java/MapboxGLAndroidSDKTestApp/build/outputs/apk
    JNILIB=`mason env JNIDIR`

    # ARM64 does not build APK for now
    if [ ${JNIDIR} != "arm64-v8a" ] ; then
        # Upload either the debug or the release build
        if [ ${BUILDTYPE} == "Debug" ] ; then
            aws s3 cp \
                ${APK_OUTPUTS}/MapboxGLAndroidSDKTestApp-${JNILIB}-debug.apk \
                ${S3_PREFIX}/MapboxGLAndroidSDKTestApp-debug.apk
        elif [ ${BUILDTYPE} == "Release" ] ; then
            aws s3 cp \
                ${APK_OUTPUTS}/MapboxGLAndroidSDKTestApp-${JNILIB}-release-unsigned.apk \
                ${S3_PREFIX}/MapboxGLAndroidSDKTestApp-release-unsigned.apk
        fi
    fi

    mapbox_time_finish
fi
