#!/usr/bin/env bash

if [ -z `which appledoc` ]; then
    echo "Unable to find appledoc. See https://github.com/mapbox/mapbox-gl-native#manually"
    exit 1
fi

OUTPUT="/tmp/`uuidgen`"

VERSION=$( git tag --sort -v:refname | sed -n '1p' | sed 's/^v//' )
echo "Creating new docs for ${VERSION}..."
echo

README="/tmp/GL-README.md"
cat ../README.md | grep -v screenshot > ${README}
echo >> ${README}
echo -n "#" >> ${README}
cat ../../CHANGELOG.md >> ${README}

appledoc \
    --output ${OUTPUT} \
    --project-name "Mapbox GL for iOS ${VERSION}" \
    --project-company Mapbox \
    --create-docset \
    --company-id com.mapbox \
    --ignore ../../include/mbgl/ios/private \
    --index-desc ${README} \
    ../../include/mbgl/ios
