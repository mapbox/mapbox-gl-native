#!/usr/bin/env bash

set -e
set -o pipefail

./scripts/local_mason.sh

echo CXX=\""`MASON_DIR=.mason MASON_PLATFORM=android .mason/mason env CXX`"\"
echo CC=\""`MASON_DIR=.mason MASON_PLATFORM=android .mason/mason env CC`"\"
echo LD=\""`MASON_DIR=.mason MASON_PLATFORM=android .mason/mason env LD`"\"
echo LINK=\""`MASON_DIR=.mason MASON_PLATFORM=android .mason/mason env CXX`"\"
echo AR=\""`MASON_DIR=.mason MASON_PLATFORM=android .mason/mason env AR`"\"
echo RANLIB=\""`MASON_DIR=.mason MASON_PLATFORM=android .mason/mason env RANLIB`"\"
echo LDFLAGS=\""`MASON_DIR=.mason MASON_PLATFORM=android .mason/mason env LDFLAGS` ${LDFLAGS}"\"
echo CFLAGS=\""`MASON_DIR=.mason MASON_PLATFORM=android .mason/mason env CFLAGS` ${CFLAGS}"\"
echo CPPFLAGS=\""`MASON_DIR=.mason MASON_PLATFORM=android .mason/mason env CPPFLAGS` ${CPPFLAGS}"\"
echo PATH=\""`MASON_DIR=.mason MASON_PLATFORM=android .mason/mason env PATH`"\"
echo JNIDIR=\""`MASON_DIR=.mason MASON_PLATFORM=android .mason/mason env JNIDIR`"\"

echo MASON_PLATFORM=\"android\"