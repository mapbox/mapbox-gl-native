#!/usr/bin/env bash

set -e
set -o pipefail

BUILDTYPE=${BUILDTYPE:-Release}

source ./scripts/travis_helper.sh

# Add Mason to PATH
export PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"

################################################################################
# Build
################################################################################

mapbox_time "package_ios" \
make ipackage

################################################################################
# Deploy
################################################################################

if [[ -n "$PUBLISH_TAG" ]]; then
    mapbox_time "deploy_ios" \
    ./scripts/ios/publish.sh "$PUBLISH_VERSION"
fi
