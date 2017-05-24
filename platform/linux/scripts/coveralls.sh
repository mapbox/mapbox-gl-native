#!/usr/bin/env bash

set -e
set -o pipefail

command -v lcov 2> /dev/null || {
    echo "Aborting: lcov not found."
    exit 1
}

# Collect coverage data and save it into coverage.info
lcov \
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

coveralls-lcov \
    --service-name="${COVERALLS_SERVICE_NAME}" \
    --repo-token="${COVERALLS_REPO_TOKEN}" \
    --service-job-id="${CIRCLE_BUILD_NUM}" \
    "build/linux-x86_64/${BUILDTYPE}/coverage.info"
