#!/usr/bin/env bash

set -e
set -o pipefail

export PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"

CLANG_TIDY=${CLANG_TIDY:-$(mason prefix clang-tidy 3.8)/bin/clang-tidy}

command -v ${CLANG_TIDY} >/dev/null 2>&1 || {
    echo "Can't find ${CLANG_TIDY} in PATH."
    if [ -z ${CLANG_TIDY} ]; then
        echo "Alternatively, you can set CLANG_TIDY to point to clang-tidy."
    fi
    exit 1
}

cd $1

git ls-files '../../../src/mbgl/*.cpp' '../../../platform/*.cpp' '../../../test/*.cpp' | \
    xargs -I{} -P ${JOBS} ${CLANG_TIDY} {} -p=. -header-filter='mbgl' || exit 1
