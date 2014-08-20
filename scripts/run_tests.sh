#!/usr/bin/env bash

set -e
set -o pipefail

cd build/Testing

for TEST in ./test_* ; do
    ${TEST}
done
