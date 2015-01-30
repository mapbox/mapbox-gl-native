#!/usr/bin/env bash

set -e
set -o pipefail

./scripts/local_mason.sh

export MASON_PLATFORM=android

echo MASON_PLATFORM=\"android\"
echo MASON_ANDROID_ABI=\"${MASON_ANDROID_ABI}\"

echo CXX=\"`.mason/mason env CXX`\"
echo CC=\"`.mason/mason env CC`\"
echo LD=\"`.mason/mason env LD`\"
echo LINK=\"`.mason/mason env CXX`\"
echo AR=\"`.mason/mason env AR`\"
echo RANLIB=\"`.mason/mason env RANLIB`\"
echo LDFLAGS=\"`.mason/mason env LDFLAGS` ${LDFLAGS}\"
echo CFLAGS=\"`.mason/mason env CFLAGS` ${CFLAGS}\"
echo CPPFLAGS=\"`.mason/mason env CPPFLAGS` ${CPPFLAGS}\"
echo PATH=\"`.mason/mason env PATH`\"
echo JNIDIR=\"`.mason/mason env JNIDIR`\"
