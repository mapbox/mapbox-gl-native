#!/usr/bin/env bash

set -e
set -o pipefail

# Collect coverage data and save it into coverage.info
mapbox_time "lcov_capture" \
`scripts/mason.sh PREFIX lcov VERSION 1.12`/usr/bin/lcov \
    --quiet \
    --capture \
    --no-external \
    --gcov-tool "gcov-5" \
    --directory "src/mbgl" \
    --directory "platform" \
    --directory "include/mbgl" \
    --directory "build/linux-x86_64/${BUILDTYPE}" \
    --base-directory "build/linux-x86_64/${BUILDTYPE}" \
    --output-file "build/linux-x86_64/${BUILDTYPE}/coverage.info"

mapbox_time "coveralls_upload" \
coveralls-lcov "build/linux-x86_64/${BUILDTYPE}/coverage.info"
