#!/usr/bin/env bash

set -e
set -o pipefail
set -u

if [ -z `which jazzy` ]; then
    echo "Installing jazzy…"
    gem install jazzy
    if [ -z `which jazzy` ]; then
        echo "Unable to install jazzy. See https://github.com/mapbox/mapbox-gl-native/blob/master/platform/osx/INSTALL.md"
        exit 1
    fi
fi

OUTPUT=${OUTPUT:-documentation}

BRANCH=$( git describe --tags --match=osx-v*.*.* --abbrev=0 )
SHORT_VERSION=$( echo ${BRANCH} | sed 's/^osx-v//' )
RELEASE_VERSION=$( echo ${SHORT_VERSION} | sed -e 's/^osx-v//' -e 's/-.*//' )

SWIFT_VERSION=$(xcrun swift -version | head -n 1 | sed -e 's/^Apple Swift version //' -e 's/ .*$//')

rm -rf /tmp/mbgl
mkdir -p /tmp/mbgl/
README=/tmp/mbgl/README.md
cp platform/osx/docs/doc-README.md "${README}"
# http://stackoverflow.com/a/4858011/4585461
echo "## Changes in version ${RELEASE_VERSION}" >> "${README}"
sed -n -e '/^## /{' -e ':a' -e 'n' -e '/^##/q' -e 'p' -e 'ba' -e '}' platform/osx/CHANGELOG.md >> "${README}"

rm -rf ${OUTPUT}
mkdir -p ${OUTPUT}

jazzy \
    --config platform/osx/jazzy.yml \
    --sdk macosx \
    --swift-version $SWIFT_VERSION \
    --github-file-prefix https://github.com/mapbox/mapbox-gl-native/tree/${BRANCH} \
    --module-version ${SHORT_VERSION} \
    --readme ${README} \
    --output ${OUTPUT}
# https://github.com/realm/jazzy/issues/411
find ${OUTPUT} -name *.html -exec \
    perl -pi -e 's/Mapbox\s+(Docs|Reference)/Mapbox OS X SDK $1/' {} \;
