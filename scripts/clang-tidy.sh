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

function check_tidy() {
    OUTPUT=$(${CLANG_TIDY} $0 -p=. -header-filter='mbgl' 2>/dev/null)
    if [[ -n $OUTPUT ]]; then
        echo "Error: A clang-tidy warning/error happened:"
        echo -e "$OUTPUT"
        exit 1
    fi
}

export CLANG_TIDY
export -f check_tidy

echo "Running clang-tidy checks... (this might take a while)"
git ls-files '../../../src/mbgl/*.cpp' '../../../platform/*.cpp' '../../../test/*.cpp' | \
    xargs -I{} -P ${JOBS} bash -c 'check_tidy' {}
