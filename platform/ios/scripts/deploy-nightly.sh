#!/usr/bin/env bash

set -e
set -o pipefail
set -u

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

export TRAVIS_REPO_SLUG=mapbox-gl-native

DATE=`date +%Y-%m-%d`
NIGHTLY_TYPE="nightly-dynamic"

step "Uploading ${NIGHTLY_TYPE} build for ${DATE} to s3…"

./platform/ios/scripts/publish.sh "${NIGHTLY_TYPE}" "${DATE}"

step "Finished deploying ${NIGHTLY_TYPE} build in $(($SECONDS / 60)) minutes and $(($SECONDS % 60)) seconds"
