#!/usr/bin/env bash

set -euo pipefail

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

#
# iOS release tag format is `vX.Y.Z`; `X.Y.Z` gets passed in
# In the case of stripped builds, we also append the `-stripped`.
#
PUBLISH_VERSION="$1"

if [[ ${#} -eq 2 ]]; then
    PUBLISH_STYLE="-$2"
else
    PUBLISH_STYLE=""
fi

GITHUB_REPO=${GITHUB_REPO:-'mapbox-gl-native'}

#
# zip
#
cd build/ios/pkg
SNAPSHOT_FILENAME="mapbox-ios-sdk-${PUBLISH_VERSION}${PUBLISH_STYLE}.zip"
step "Compressing ${SNAPSHOT_FILENAME}…"
rm -f ../${SNAPSHOT_FILENAME}
zip -yr ../${SNAPSHOT_FILENAME} *

#
# report file sizes
#
step "Echoing file sizes…"
du -sh ../${SNAPSHOT_FILENAME}
du -sch *
du -sch dynamic/*

#
# upload
#
PROGRESS=""
if [ -n "${CI:-}" ]; then
    PROGRESS="--no-progress"
fi

step "Uploading ${SNAPSHOT_FILENAME} to s3…"
aws s3 cp ../${SNAPSHOT_FILENAME} s3://mapbox/$GITHUB_REPO/ios/builds/ --acl public-read ${PROGRESS}
echo "URL: https://mapbox.s3.amazonaws.com/$GITHUB_REPO/ios/builds/${SNAPSHOT_FILENAME}"

#
# upload & update snapshot
#
if [[ ${PUBLISH_VERSION} =~ "snapshot" ]]; then
    step "Updating ${PUBLISH_VERSION} to ${PUBLISH_STYLE}…"
    GENERIC_SNAPSHOT_FILENAME="mapbox-ios-sdk-${PUBLISH_VERSION}.zip"
    aws s3 cp \
        s3://mapbox/$GITHUB_REPO/ios/builds/${SNAPSHOT_FILENAME} \
        s3://mapbox/$GITHUB_REPO/ios/builds/${GENERIC_SNAPSHOT_FILENAME} --acl public-read ${PROGRESS}
fi
