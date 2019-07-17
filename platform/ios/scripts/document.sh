#!/usr/bin/env bash

set -euo pipefail

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

if [ -z `which jazzy` ]; then
    ./platform/ios/scripts/install-packaging-dependencies.sh
fi

DEFAULT_THEME="platform/darwin/docs/theme"
THEME=${JAZZY_THEME:-$DEFAULT_THEME}
CUSTOM_HEAD=${JAZZY_CUSTOM_HEAD:-''}
OUTPUT=${OUTPUT:-documentation}

BRANCH=$( git describe --tags --match=ios-v*.*.* --abbrev=0 )
SHORT_VERSION=$( echo ${BRANCH} | sed 's/^ios-v//' )
RELEASE_VERSION=$( echo ${SHORT_VERSION} | sed -e 's/^ios-v//' -e 's/-.*//' )

rm -rf /tmp/mbgl
mkdir -p /tmp/mbgl/

step "Generating readme and release notes"
README=/tmp/mbgl/README.md
npm install --ignore-scripts
node platform/ios/scripts/release-notes.js jazzy >> "${README}"

rm -rf ${OUTPUT}
mkdir -p ${OUTPUT}

cp -r platform/darwin/docs/img "${OUTPUT}"
cp -r platform/ios/docs/img "${OUTPUT}"

step "Generating jazzy docs for ${SHORT_VERSION}…"

jazzy \
    --config platform/ios/jazzy.yml \
    --sdk iphonesimulator \
    --github-file-prefix https://github.com/mapbox/mapbox-gl-native/tree/${BRANCH} \
    --module-version ${SHORT_VERSION} \
    --readme ${README} \
    --documentation="platform/{darwin,ios}/docs/guides/*.md" \
    --root-url https://docs.mapbox.com/ios/api/maps/${RELEASE_VERSION}/ \
    --theme ${THEME} \
    --head "${CUSTOM_HEAD}" \
    --output ${OUTPUT} \
    --title "Maps SDK for iOS" \
    --module-version ${SHORT_VERSION}
