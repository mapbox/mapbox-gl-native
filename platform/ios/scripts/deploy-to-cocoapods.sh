#!/usr/bin/env bash

# This relies on either:
#   1. You being authenticated locally with CocoaPods trunk.
#   2. The `COCOAPODS_TRUNK_TOKEN` environment variable being set.

set -euo pipefail

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

CIRCLE_TAG=${CIRCLE_TAG:-""}

step "Pushing release to CocoaPods trunk…"

if [[ $CIRCLE_TAG ]]; then
  pod trunk push platform/ios/Mapbox-iOS-SDK.podspec --allow-warnings
else
  echo "Skipping push to CocoaPods trunk for untagged build"
fi

step "Pushing release/builds to Mapbox podspecs repo…"

if [[ -z $(pod repo list | grep -i mapbox-public) ]]; then
  pod repo add mapbox-public https://github.com/mapbox/pod-specs
else
  echo "Found existing mapbox-public podspecs repo"
fi

if [[ -z $(git config --global user.email) && $CI ]]; then
  echo "Setting machine user as git committer"
  git config --global user.email "MapboxCI@users.noreply.github.com"
fi

if [[ $CIRCLE_TAG ]]; then
  pod repo push mapbox-public platform/ios/Mapbox-iOS-SDK.podspec --allow-warnings
  pod repo push mapbox-public platform/ios/Mapbox-iOS-SDK-stripped.podspec --allow-warnings
else
  echo "Skipping push of release podspecs to mapbox-public for untagged build"

  # pod repo push mapbox-public platform/ios/Mapbox-iOS-SDK-snapshot-dynamic.podspec --allow-warnings
  echo "Skipping push of snapshot to mapbox-public until we have a way to update the versions in the snapshot podspec"
fi
