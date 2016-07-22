#!/usr/bin/env bash

set -e
set -o pipefail

export MASON_PLATFORM=android
export MASON_ANDROID_ABI=${1:-arm-v7}
export PATH="`pwd`/.mason:${PATH}"
export MASON_DIR="`pwd`/.mason"

export PATH=`mason env PATH`

echo "set(CMAKE_SYSTEM_NAME Android)"
echo "set(CMAKE_SYSTEM_VERSION 1)"
echo "set(CMAKE_CXX_COMPILER \"`which $(mason env CXX)`\")"
echo "set(CMAKE_C_COMPILER \"`which $(mason env CC)`\")"
echo "set(ANDROID_JNIDIR \"`mason env JNIDIR`\")"
echo "set(ANDROID_ABI \"${1:-arm-v7}\")"
echo "set(ENV{MASON_ANDROID_ABI} \"${1:-arm-v7}\")"
echo "set(CMAKE_EXE_LINKER_FLAGS \"`mason env LDFLAGS` \${CMAKE_EXE_LINKER_FLAGS}\")"
echo "set(CMAKE_CXX_FLAGS \"-s -fvisibility=hidden -Os `mason env CXXFLAGS` \${CMAKE_CXX_FLAGS}\")"
echo "set(CMAKE_C_FLAGS \"-s -fvisibility=hidden -Os `mason env CPPFLAGS` \${CMAKE_C_FLAGS}\")"
