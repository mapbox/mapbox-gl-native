#!/usr/bin/env bash

set -e
set -o pipefail
set -u

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

if [ -z `which jazzy` ]; then
    step "Installing jazzy…"

    CIRCLECI=${CIRCLECI:-false}
    if [[ "${CIRCLECI}" == true ]]; then
        sudo gem install jazzy --no-document
    else
        gem install jazzy --no-document
    fi

    if [ -z `which jazzy` ]; then
        echo "Unable to install jazzy. See https://github.com/mapbox/mapbox-gl-native/blob/master/platform/ios/INSTALL.md"
        exit 1
    fi
fi

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
DEFAULT_THEME="platform/darwin/docs/theme"
THEME=${JAZZY_THEME:-$DEFAULT_THEME}

jazzy \
    --config platform/ios/jazzy.yml \
    --sdk iphonesimulator \
    --github-file-prefix https://github.com/mapbox/mapbox-gl-native/tree/${BRANCH} \
    --module-version ${SHORT_VERSION} \
    --readme ${README} \
    --documentation="platform/{darwin,ios}/docs/guides/*.md" \
    --root-url https://www.mapbox.com/ios-sdk/api/${RELEASE_VERSION}/ \
    --theme ${THEME} \
    --output ${OUTPUT}

# https://github.com/realm/jazzy/issues/411
find ${OUTPUT} -name *.html -exec \
    perl -pi -e 's/BRANDLESS_DOCSET_TITLE/iOS SDK $1/, s/Mapbox\s+(Docs|Reference)/Mapbox Maps SDK for iOS $1/' {} \;
