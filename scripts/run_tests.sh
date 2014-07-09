#!/usr/bin/env bash

set -e
set -o pipefail

BUILDTYPE="${BUILDTYPE:-Release}"

cd build/${BUILDTYPE}

for TEST in ./test_* ; do
    ${TEST}
done
