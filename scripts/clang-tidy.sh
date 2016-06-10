#!/usr/bin/env bash

set -e
set -o pipefail

directory=$1

export PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"

command -v ${CLANG_TIDY:-clang-tidy} >/dev/null 2>&1 || {
    echo "Can't find ${CLANG_TIDY:-clang-tidy} in PATH."
    if [ -z ${CLANG_TIDY} ]; then
        echo "Alternatively, you can set CLANG_TIDY to point to clang-tidy."
    fi
    if [ `uname -s` = 'Linux' ]; then
        echo "On Debian-based distros, you can install them via 'apt-get install clang-tidy'"
    elif [ `uname -s` = 'Darwin' ]; then
        echo "On OS X, you can install them via 'brew install llvm --with-clang --with-clang-extra-tools'"
    fi
    exit 1
}

cd "${directory}"

git ls-files '../../../src/mbgl/*.cpp' '../../../platform/*.cpp' '../../../test/*.cpp' | \
    xargs -I{} -P ${JOBS} ${CLANG_TIDY:-clang-tidy} -header-filter='\/mbgl\/' {}
