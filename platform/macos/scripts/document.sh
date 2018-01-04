#!/usr/bin/env bash

set -e
set -o pipefail
set -u

if [ -z `which jazzy` ]; then
    echo "Installing jazzy…"
    gem install jazzy
    if [ -z `which jazzy` ]; then
        echo "Unable to install jazzy. See https://github.com/mapbox/mapbox-gl-native/blob/master/platform/macos/INSTALL.md"
        exit 1
    fi
fi

OUTPUT=${OUTPUT:-documentation}

BRANCH=$( git describe --tags --match=macos-v*.*.* --abbrev=0 )
SHORT_VERSION=$( echo ${BRANCH} | sed 's/^macos-v//' )
RELEASE_VERSION=$( echo ${SHORT_VERSION} | sed -e 's/^macos-v//' -e 's/-.*//' )

rm -rf /tmp/mbgl
mkdir -p /tmp/mbgl/
README=/tmp/mbgl/README.md
if [[ ${STANDALONE:-} ]]; then
    cp platform/macos/docs/pod-README.md "${README}"
    perl -pi -e 's|https://raw.githubusercontent.com/mapbox/mapbox-gl-native/master/platform/macos/docs/||' \
        "${README}"
else
    cp platform/macos/docs/doc-README.md "${README}"
fi
# http://stackoverflow.com/a/4858011/4585461
echo "## Changes in [version ${RELEASE_VERSION}](https://github.com/mapbox/mapbox-gl-native/releases/tag/${BRANCH})" >> "${README}"
sed -n -e '/^## /{' -e ':a' -e 'n' -e '/^## /q' -e 'p' -e 'ba' -e '}' platform/macos/CHANGELOG.md >> "${README}"

rm -rf ${OUTPUT}
mkdir -p ${OUTPUT}

cp -r platform/darwin/docs/img "${OUTPUT}"
cp -r platform/macos/docs/img "${OUTPUT}"

jazzy \
    --config platform/macos/jazzy.yml \
    --sdk macosx \
    --github-file-prefix https://github.com/mapbox/mapbox-gl-native/tree/${BRANCH} \
    --module-version ${SHORT_VERSION} \
    --readme ${README} \
    --documentation="platform/{darwin,macos}/docs/guides/*.md" \
    --theme platform/darwin/docs/theme \
    --output ${OUTPUT}
# https://github.com/realm/jazzy/issues/411
find ${OUTPUT} -name *.html -exec \
    perl -pi -e 's/BRANDLESS_DOCSET_TITLE/Maps SDK for macOS $1/, s/Mapbox\s+(Docs|Reference)/Mapbox Maps SDK for macOS $1/' {} \;
