#!/usr/bin/env bash

set -e
set -o pipefail

if [ -z ${WITH_COVERAGE} ] ; then
    echo "WITH_COVERAGE environment variable is not set, aborting."
    exit 1
fi

directory=$1

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

# Zero coverage counters
lcov \
    --quiet \
    --zerocounters \
    --directory "${directory}" \
    --output-file "${directory}/coverage.info" \
    >/dev/null 2>&1

# Run all unit tests
make

# Collect coverage data and save it into coverage.info
echo "Collecting coverage data..."
lcov \
    --quiet \
    --capture \
    --no-external \
    --directory "src/mbgl" \
    --directory "platform" \
    --directory "include/mbgl" \
    --directory "${directory}" \
    --base-directory "${directory}" \
    --output-file "${directory}/coverage.info" \
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
    --output-directory "${directory}/coverage" \
    "${directory}/coverage.info" \
    >/dev/null 2>&1

echo "Coverage report is now available in ${directory}/coverage/index.html"
