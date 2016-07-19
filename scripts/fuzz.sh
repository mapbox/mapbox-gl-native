#!/usr/bin/env bash

set -e
set -o pipefail

export PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"

AFL_PREFIX=$(mason prefix afl 2.19b)
AFL_FUZZ=${AFL_PREFIX}/bin/afl-fuzz


if [ -z "$2" ]; then
    echo "Usage: $0 <build path> <name>"
    exit 1
fi

if [ ! -d "test/fuzz-input/$2" ]; then
    echo "Fuzzing data for '$2' does not exist."
    exit 1
fi

FUZZ=$1/fuzz
${FUZZ} $2 < /dev/null

rm -rf test/fuzz-result/$2
mkdir -p test/fuzz-result/$2

${AFL_FUZZ} -i test/fuzz-input/$2 -o test/fuzz-result/$2 ${FUZZ} $2
