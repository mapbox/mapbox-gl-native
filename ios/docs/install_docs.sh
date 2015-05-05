#!/bin/sh

if [ -z `which appledoc` ]; then
    echo "Unable to find appledoc. Consider installing it from source or Homebrew."
    exit 1
fi

OUTPUT="/tmp/`uuidgen`"

VERSION=$( git tag -l ios\* --sort -v:refname | sed -n '1p' | sed 's/ios-v//' )
echo "Creating new docs for $VERSION..."
echo

appledoc \
    --output $OUTPUT \
    --project-name "Mapbox GL for iOS $VERSION" \
    --project-company Mapbox \
    --create-docset \
    --company-id com.mapbox \
    --ignore ../../include/mbgl/ios/private \
    ../../include/mbgl/ios
