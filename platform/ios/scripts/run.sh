#!/usr/bin/env bash

set -e
set -o pipefail
set -u

source ./platform/ios/scripts/setup.sh

BUILDTYPE=${BUILDTYPE:-Release}

PUBLISH_TAG=($(git show -s --format=%B | sed -n 's/.*\[publish \([a-z]\{1,\}\)-v\([0-9a-z.\-]\{1,\}\)\].*/\1 \2/p'))
PUBLISH_PLATFORM=${PUBLISH_TAG[0],-}
PUBLISH_VERSION=${PUBLISH_TAG[1],-}


################################################################################
# Build
################################################################################

if [[ ${PUBLISH_PLATFORM} = 'ios' ]]; then
    # default, with debug symbols
    mapbox_time "package_ios_symbols" \
    make ipackage

    mapbox_time "deploy_ios_symbols"
    ./platform/ios/scripts/publish.sh "${PUBLISH_VERSION}" symbols

    # no debug symbols, for smaller distribution
    mapbox_time "package_ios_stripped" \
    make ipackage-strip

    mapbox_time "deploy_ios_stripped"
    ./platform/ios/scripts/publish.sh "${PUBLISH_VERSION}"
    
    # dynamic, with debug symbols
    mapbox_time "package_ios_dynamic" \
    make iframework

    mapbox_time "deploy_ios_dynamic"
    ./platform/ios/scripts/publish.sh "${PUBLISH_VERSION}" symbols-dynamic

    # dynamic, without debug symbols
    mapbox_time "package_ios_dynamic_stripped" \
    make iframework SYMBOLS=NO

    mapbox_time "deploy_ios_dynamic_stripped"
    ./platform/ios/scripts/publish.sh "${PUBLISH_VERSION}" dynamic
else
    # build & test iOS
    mapbox_time "run_ios_tests" \
    make itest
fi
