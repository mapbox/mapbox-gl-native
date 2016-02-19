#!/usr/bin/env bash

set -e
set -o pipefail

PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"

mapbox_time "install_lcov" \
mason install lcov 1.12

LCOV="`mason prefix lcov 1.12`/usr/bin/lcov"

mapbox_time "check_mason_lcov" \
command -v $LCOV > /dev/null 2>&1 || exit 1

mapbox_time "remove_coverage_info" \
rm -f "build/${HOST_SLUG}/${BUILDTYPE}/coverage.info"

# Run all unit tests
./scripts/run_tests.sh "build/${HOST_SLUG}/${BUILDTYPE}/test"

# Collect coverage data and save it into coverage.info
mapbox_time "lcov_capture" \
$LCOV \
    --quiet \
    --capture \
    --no-external \
    --gcov-tool "gcov-4.9" \
    --directory "src/mbgl" \
    --directory "platform" \
    --directory "include/mbgl" \
    --directory "build/${HOST_SLUG}/${BUILDTYPE}" \
    --base-directory "build/${HOST_SLUG}/${BUILDTYPE}" \
    --output-file "build/${HOST_SLUG}/${BUILDTYPE}/coverage.info"

mapbox_time "coveralls_upload" \
coveralls-lcov "build/${HOST_SLUG}/${BUILDTYPE}/coverage.info"
