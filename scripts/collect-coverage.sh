#!/usr/bin/env bash

set -e
set -o pipefail

if [ -z ${WITH_COVERAGE} ] ; then
    echo "WITH_COVERAGE environment variable is not set, aborting."
    exit 1
fi

SOURCE_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/../"; pwd)

function usage() {
    echo "Error: LCOV and genhtml are required for generating coverage reports."
    if [ `uname -s` = 'Linux' ]; then
        echo "On Debian-based distros, you can install them via 'apt-get install lcov'"
    elif [ `uname -s` = 'Darwin' ]; then
        echo "On macOS, you can install them via 'brew install lcov'"
    fi
    exit 1
}

command -v lcov >/dev/null 2>&1 || usage
command -v genhtml >/dev/null 2>&1 || usage

# Build unit tests
make test

# Zero coverage counters
lcov \
    --quiet \
    --zerocounters \
    --directory "${SOURCE_DIR}" \
    --output-file "${SOURCE_DIR}/coverage.info" \
    >/dev/null 2>&1

# Run all unit tests
make run-test

# Collect coverage data and save it into coverage.info
echo "Collecting coverage data..."
lcov \
    --quiet \
    --capture \
    --no-external \
    --directory "${SOURCE_DIR}" \
    --output-file "${SOURCE_DIR}/coverage.info" \
    >/dev/null 2>&1

# Remove coverage data for unwanted files from a tracefile
lcov \
    --quiet \
    --remove "${SOURCE_DIR}/coverage.info" \
    "${SOURCE_DIR}/vendor/*" \
    "${SOURCE_DIR}/mason/*" \
    "${SOURCE_DIR}/test/*" \
    "/usr/include/*" \
    --output-file "${SOURCE_DIR}/total_coverage.info" \
    >/dev/null 2>&1

# Generate HTML report based on total_coverage.info
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
    --output-directory "${SOURCE_DIR}/coverage" \
    "${SOURCE_DIR}/total_coverage.info" \
    >/dev/null 2>&1

echo "Coverage report is now available in ${SOURCE_DIR}/coverage/index.html"
