#!/usr/bin/env bash

set -e
set -o pipefail

export MASON_PLATFORM=android
export MASON_ANDROID_ABI=${MASON_ANDROID_ABI:-arm-v7}
export PATH="`pwd`/.mason:${PATH}"
export MASON_DIR="`pwd`/.mason"

export PATH=`mason env PATH`

echo MASON_PLATFORM=\"${MASON_PLATFORM}\"
echo MASON_ANDROID_ABI=\"${MASON_ANDROID_ABI}\"
echo CXX=\"`which $(mason env CXX)`\"
echo CC=\"`which $(mason env CC)`\"
echo LD=\"`which $(mason env LD)`\"
echo LINK=\"`which $(mason env CXX)`\"
echo AR=\"`which $(mason env AR)`\"
echo RANLIB=\"`which $(mason env RANLIB)`\"
echo STRIP=\"`which $(mason env STRIP)`\"
echo LDFLAGS=\"`mason env LDFLAGS` \${LDFLAGS:-}\"
echo CFLAGS=\"`mason env CFLAGS` \${CFLAGS:-}\"
echo CXXFLAGS=\"`mason env CXXFLAGS` \${CXXFLAGS:-}\"
echo CPPFLAGS=\"`mason env CPPFLAGS` \${CPPFLAGS:-}\"
echo JNIDIR=\"`mason env JNIDIR`\"
