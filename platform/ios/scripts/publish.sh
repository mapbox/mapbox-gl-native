#!/usr/bin/env bash

set -e
set -o pipefail
set -u

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

#
# iOS release tag format is `vX.Y.Z`; `X.Y.Z` gets passed in
# In the case of symbolicated builds, we also append the `-symbols`.
#
PUBLISH_VERSION="$1"

if [[ ${#} -eq 2 ]]; then
    PUBLISH_STYLE="-$2"
else
    PUBLISH_STYLE=""
fi

#
# zip
#
cd build/ios/pkg
ZIP="mapbox-ios-sdk-${PUBLISH_VERSION}${PUBLISH_STYLE}.zip"
step "Compressing ${ZIP}…"
rm -f ../${ZIP}
zip -yr ../${ZIP} *

#
# upload
#
step "Uploading ${ZIP} to s3…"
REPO_NAME=$(basename $TRAVIS_REPO_SLUG)
aws s3 cp ../${ZIP} s3://mapbox/$REPO_NAME/ios/builds/ --acl public-read
echo "URL: https://mapbox.s3.amazonaws.com/$REPO_NAME/ios/builds/${ZIP}"

#
# update nightly
#
if [[ ${PUBLISH_VERSION} =~ "nightly" ]]; then
    step "Updating ${PUBLISH_VERSION} to ${PUBLISH_STYLE}…"
    GENERIC_NIGHTLY_FILENAME="mapbox-ios-sdk-${PUBLISH_VERSION}.zip"
    aws s3 cp \
        s3://mapbox/$REPO_NAME/ios/builds/${ZIP} \
        s3://mapbox/$REPO_NAME/ios/builds/${GENERIC_NIGHTLY_FILENAME} --acl public-read
fi
