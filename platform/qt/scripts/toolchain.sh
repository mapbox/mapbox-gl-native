#!/usr/bin/env bash

set -e
set -o pipefail

if [ -z ${MASON_SYSROOT:-} ]; then
    exit 0
fi

export PATH="`pwd`/.mason:${PATH}"
export MASON_DIR="`pwd`/.mason"

export PATH=`mason env PATH`

echo CXX=\"`which $(mason env CXX)`\"
echo CC=\"`which $(mason env CC)`\"
echo LD=\"`which $(mason env LD)`\"
echo LINK=\"`which $(mason env CXX)`\"
echo AR=\"`which $(mason env AR)`\"
echo RANLIB=\"`which $(mason env RANLIB)`\"
echo STRIP=\"`which $(mason env STRIP)`\"
