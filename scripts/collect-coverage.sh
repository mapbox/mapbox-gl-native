#!/usr/bin/env bash

set -e
set -o pipefail

function usage() {
    echo "Error: LCOV and genhtml are required for generating coverage reports."
    if [ `uname -s` = 'Linux' ]; then
        echo "On Debian-based distros, you can install them via 'apt-get install lcov'"
    elif [ `uname -s` = 'Darwin' ]; then
        echo "On OS X, you can install them via 'brew install lcov'"
    fi
    exit 1
}

command -v lcov >/dev/null 2>&1 || usage
command -v genhtml >/dev/null 2>&1 || usage

# Zero coverage counters
lcov \
    --quiet \
    --zerocounters \
    --directory "build/${HOST_SLUG}/${BUILDTYPE}" \
    --output-file "build/${HOST_SLUG}/${BUILDTYPE}/coverage.info" \
    >/dev/null 2>&1

# Run all unit tests
./scripts/run_tests.sh "build/${HOST_SLUG}/${BUILDTYPE}/test"

# Collect coverage data and save it into coverage.info
echo "Collecting coverage data..."
lcov \
    --quiet \
    --capture \
    --no-external \
    --directory "src/mbgl" \
    --directory "platform" \
    --directory "include/mbgl" \
    --directory "build/${HOST_SLUG}/${BUILDTYPE}" \
    --base-directory "build/${HOST_SLUG}/${BUILDTYPE}" \
    --output-file "build/${HOST_SLUG}/${BUILDTYPE}/coverage.info" \
    >/dev/null 2>&1

# Generate HTML report based on coverage.info
echo "Generating HTML report..."
genhtml \
    --quiet \
    --show-details \
    --keep-descriptions \
    --function-coverage \
    --no-branch-coverage \
    --title "Mapbox GL Native" \
    --num-spaces 4 \
    --sort \
    --demangle-cpp \
    --prefix $(pwd -P) \
    --output-directory "build/${HOST_SLUG}/${BUILDTYPE}/coverage" \
    "build/${HOST_SLUG}/${BUILDTYPE}/coverage.info" \
    >/dev/null 2>&1

echo "Coverage report is now available in build/${HOST_SLUG}/${BUILDTYPE}/coverage/index.html"
