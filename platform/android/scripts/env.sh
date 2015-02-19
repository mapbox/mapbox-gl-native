#!/usr/bin/env bash

set -e
set -o pipefail

./scripts/local_mason.sh

export MASON_PLATFORM=android

export PATH=`.mason/mason env PATH`

echo MASON_PLATFORM=\"android\"
echo MASON_ANDROID_ABI=\"${MASON_ANDROID_ABI}\"

echo CXX=\"`which $(.mason/mason env CXX)`\"
echo CC=\"`which $(.mason/mason env CC)`\"
echo LD=\"`which $(.mason/mason env LD)`\"
echo LINK=\"`which $(.mason/mason env CXX)`\"
echo AR=\"`which $(.mason/mason env AR)`\"
echo RANLIB=\"`which $(.mason/mason env RANLIB)`\"
echo STRIP=\"`which $(.mason/mason env STRIP)`\"
echo LDFLAGS=\"`.mason/mason env LDFLAGS` ${LDFLAGS}\"
echo CFLAGS=\"`.mason/mason env CFLAGS` ${CFLAGS}\"
echo CPPFLAGS=\"`.mason/mason env CPPFLAGS` ${CPPFLAGS}\"
echo JNIDIR=\"`.mason/mason env JNIDIR`\"
