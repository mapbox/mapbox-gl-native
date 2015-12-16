#!/usr/bin/env bash

set -e
set -o pipefail
set -u

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
cd build/ios/pkg/static
ZIP=mapbox-ios-sdk-${PUBLISH_VERSION}${PUBLISH_STYLE}.zip
rm -f ../${ZIP}
zip -r ../${ZIP} *
#
# upload
#
REPO_NAME=$(basename $TRAVIS_REPO_SLUG)
aws s3 cp ../${ZIP} s3://mapbox/$REPO_NAME/ios/builds/ --acl public-read > /dev/null
echo http://mapbox.s3.amazonaws.com/$REPO_NAME/ios/builds/${ZIP}
