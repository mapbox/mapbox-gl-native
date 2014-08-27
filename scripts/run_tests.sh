#!/usr/bin/env bash

set -e
set -o pipefail

cd build/${BUILDTYPE:-Release}

for TEST in ./test_* ; do
    ${TEST}
done
