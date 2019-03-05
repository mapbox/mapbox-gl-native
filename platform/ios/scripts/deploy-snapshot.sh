#!/usr/bin/env bash

set -euo pipefail

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

DATE=`date +%Y-%m-%d`
PROJ_VERSION=$(git rev-list --count HEAD)
SNAPSHOT_TYPE="snapshot-dynamic"

step "Uploading ${SNAPSHOT_TYPE} build for ${DATE}-${PROJ_VERSION} to s3…"

./platform/ios/scripts/publish.sh "${SNAPSHOT_TYPE}" "${DATE}-${PROJ_VERSION}"

step "Finished deploying ${SNAPSHOT_TYPE} build in $(($SECONDS / 60)) minutes and $(($SECONDS % 60)) seconds"
