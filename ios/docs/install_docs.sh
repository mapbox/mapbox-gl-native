#!/usr/bin/env bash

if [ -z `which appledoc` ]; then
    echo "Unable to find appledoc. See https://github.com/mapbox/mapbox-gl-native/blob/master/INSTALL.md"
    exit 1
fi

OUTPUT="/tmp/`uuidgen`"

DOCS_VERSION=$( git tag | grep ^ios | sed 's/^ios-//' | sort -r | grep -v '\-rc.' | grep -v '\-pre.' | sed -n '1p' | sed 's/^v//' )
echo "Creating new docs for ${DOCS_VERSION}..."
echo

rm -rf /tmp/mbgl
mkdir -p /tmp/mbgl/
README=/tmp/mbgl/GL-README.md
cat ./pod-README.md > ${README}
echo >> ${README}
echo -n "#" >> ${README}
cat ../../CHANGELOG.md >> ${README}
# Copy headers to a temporary location where we can substitute macros that appledoc doesn't understand.
cp -r ../../include/mbgl/ios /tmp/mbgl
perl \
    -pi \
    -e 's/NS_(?:(MUTABLE)_)?(ARRAY|SET|DICTIONARY)_OF\(\s*(.+?)\s*\)/NS\L\u$1\u$2\E <$3>/g' \
    /tmp/mbgl/ios/*.h
appledoc \
    --output ${OUTPUT} \
    --project-name "Mapbox iOS SDK ${DOCS_VERSION}" \
    --project-company Mapbox \
    --create-docset \
    --company-id com.mapbox \
    --index-desc ${README} \
    /tmp/mbgl/ios
