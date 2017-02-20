#!/usr/bin/env bash

set -e
set -o pipefail
set -u

# This script produces an env.sh file, which contains the paths to CMake, and the flags required to
# create a build. It first tries to use the Android NDK, but falls back to installing it via Mason.

function error { >&2 echo -e "\033[1m\033[31m$@\033[0m"; }
function warning { >&2 echo -e "\033[1m\033[33m$@\033[0m"; }
function status { >&2 echo -e "\033[1m\033[36m$@\033[0m"; }
function info { >&2 echo -e "\033[1m\033[32m$@\033[0m"; }

if [ "$#" -ne 3 ]; then
    error "Usage: $0 <short arch> <long arch> <api level>"
fi

NDK_ANDROID_VERSION=$1-$3
ANDROID_NATIVE_API_LEVEL=$3
ANDROID_ABI=$2

function mason_ndk {
    local CMAKE=${CMAKE:-cmake}
    MASON_XC_ROOT="`${CMAKE} -P cmake/mason.cmake PREFIX android-ndk VERSION ${NDK_ANDROID_VERSION}-r13b`"

    local TOOLCHAIN="${MASON_XC_ROOT}/toolchain.cmake"
    if [ ! -f "${TOOLCHAIN}" ]; then
        error "Can't find CMake toolchain file at ${TOOLCHAIN}."
        exit 1
    fi

    info "Using Mason-provided Android NDK at ${MASON_XC_ROOT}"
    echo CMAKE=\"${CMAKE}\"
    echo CMAKE_GENERATOR=\"Ninja\"
    echo CMAKE_ARGS=\" \
        -DCMAKE_MAKE_PROGRAM=`pwd`/${NINJA} \
        -DMASON_XC_ROOT=${MASON_XC_ROOT} \
        -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN} \
    \"
}

function system_ndk {
    if [[ ${USE_MASON_NDK:-} ]]; then
        return 1
    fi

    if [ -f platform/android/local.properties ]; then
        local SDK_DIR=$(sed -n -e 's/^sdk.dir=\(.*\)$/\1/p' platform/android/local.properties)
    fi

    if [ ! -d "${SDK_DIR:-}" ]; then
        if [ ! -z "${ANDROID_HOME:-}" ]; then
            local SDK_DIR="${ANDROID_HOME}"
        else
            error "Can't find the Android SDK. Set \$ANDROID_HOME to the SDK path."
            exit 1
        fi
    fi

    local NDK_DIR="${ANDROID_NDK_HOME:-${SDK_DIR}/ndk-bundle}"
    if [ ! -d "${NDK_DIR}" ]; then
        warning "Can't find the Android NDK. If it is installed, set \$ANDROID_NDK_HOME to the NDK path."
        return 1
    fi

    # Try to install CMake if it's not installed yet.
    mkdir -p "${SDK_DIR}/cmake"
    local CMAKE_VERSION=/$(ls "${SDK_DIR}/cmake" | sort -t. -k 1,1n -k 2,2n -k 3,3n -k 4,4n | tail -n 1)
    local CMAKE="${SDK_DIR}/cmake${CMAKE_VERSION:-}/bin/cmake"
    if [ ! -f "${CMAKE}" ]; then
        status "Trying to install CMake..."
        mkdir -p "${SDK_DIR}/licenses"
        echo "8933bad161af4178b1185d1a37fbf41ea5269c55" > "${SDK_DIR}/licenses/android-sdk-license"
        "${SDK_DIR}/tools/bin/sdkmanager" --list | grep cmake | tail -n 1 | cut -d \| -f 1 | xargs "${SDK_DIR}/tools/bin/sdkmanager" >&2
        CMAKE_VERSION=/$(ls "${SDK_DIR}/cmake" | sort -t. -k 1,1n -k 2,2n -k 3,3n -k 4,4n | tail -n 1)
        CMAKE="${SDK_DIR}/cmake${CMAKE_VERSION:-}/bin/cmake"
        if [ ! -f "${CMAKE}" ]; then
            error "Can't find CMake at ${CMAKE}."
            return 1
        fi
    fi

    local NINJA="${SDK_DIR}/cmake${CMAKE_VERSION:-}/bin/ninja"
    if [ ! -f "${NINJA}" ]; then
        error "Can't find Ninja at ${NINJA}."
        return 1
    fi

    local TOOLCHAIN="${NDK_DIR}/build/cmake/android.toolchain.cmake"
    if [ ! -f "${TOOLCHAIN}" ]; then
        error "Can't find CMake toolchain file at ${TOOLCHAIN}."
        return 1
    fi

    info "Using system-provided Android NDK at ${NDK_DIR}"
    echo CMAKE=\"${CMAKE}\"
    echo CMAKE_GENERATOR=\"Android Gradle - Ninja\"
    echo CMAKE_ARGS=\" \
        -DANDROID_ABI=${ANDROID_ABI} \
        -DANDROID_NDK=${NDK_DIR} \
        -DCMAKE_MAKE_PROGRAM=${NINJA} \
        -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN} \
        -DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL} \
        -DANDROID_TOOLCHAIN=clang \
        -DANDROID_STL=c++_static \
        -DANDROID_CPP_FEATURES=rtti\;exceptions \
    \"
}

system_ndk || mason_ndk
