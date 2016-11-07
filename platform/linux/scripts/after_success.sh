#!/usr/bin/env bash

set -e
set -o pipefail

if [ "${TRAVIS_PULL_REQUEST_BRANCH:-${TRAVIS_BRANCH}}" = "master" ] && [ "${BUILDTYPE}" = "Release" ]; then
    CLOUDWATCH=true platform/linux/scripts/metrics.sh
else
    echo "Not logging binary size for branch or Debug build"
fi
