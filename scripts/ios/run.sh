#!/usr/bin/env bash

set -e
set -o pipefail
set -u

source ./scripts/ios/setup.sh

BUILDTYPE=${BUILDTYPE:-Release}

PUBLISH_TAG=($(git show -s --format=%B | sed -n 's/.*\[publish \([a-z]\{1,\}\)-v\([0-9.]\{1,\}\)\].*/\1 \2/p'))
PUBLISH_PLATFORM=${PUBLISH_TAG[0],-}
PUBLISH_VERSION=${PUBLISH_TAG[1],-}


################################################################################
# Build
################################################################################

if [[ ${PUBLISH_PLATFORM} = 'ios' ]]; then
    # build & package iOS
    mapbox_time "package_ios" \
    make ipackage

    # publish iOS build
    mapbox_time "deploy_ios" \
    ./scripts/ios/publish.sh "${PUBLISH_VERSION}"
else
    # build & test iOS
    mapbox_time "run_ios_tests" \
    make itest
fi
