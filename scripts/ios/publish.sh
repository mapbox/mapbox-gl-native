#!/usr/bin/env bash

set -e
set -o pipefail
set -u

#
# iOS release tag format is `vX.Y.Z`; `X.Y.Z` gets passed in
#
PUBLISH_VERSION="$1"

#
# zip
#
cd build/ios/pkg/static
ZIP=mapbox-gl-ios-${PUBLISH_VERSION}.zip
rm -f ../${ZIP}
zip -r ../${ZIP} *
#
# upload
#
REPO_NAME=$(basename $TRAVIS_REPO_SLUG)
aws s3 cp ../${ZIP} s3://mapbox/$REPO_NAME/ios/builds/ --acl public-read > /dev/null
echo http://mapbox.s3.amazonaws.com/$REPO_NAME/ios/builds/${ZIP}
