#!/usr/bin/env bash

set -e
set -o pipefail
set -u

cd build/ios/pkg
rm -rf framework
mkdir framework
cd framework

mkdir Mapbox.framework

# headers
cp -rv ../static/Headers Mapbox.framework/Headers
cat ../../../../ios/framework/umbrella >> Mapbox.framework/Headers/Mapbox.h

# resources
cp -rv ../static/Mapbox.bundle Mapbox.framework/Mapbox.bundle

# settings bundle
cp -rv ../../../../ios/framework/Settings.bundle Mapbox.framework/Settings.bundle

# binary
cp -v ../static/libMapbox.a Mapbox.framework/Mapbox

# module map
mkdir Mapbox.framework/Modules
cp -v ../../../../ios/framework/modulemap Mapbox.framework/Modules/module.modulemap

# Info.plist
VERSION=$( git tag | grep ^ios | sed 's/^ios-//' | sort -r | grep -v '\-rc.' | grep -v '\-pre.' | sed -n '1p' | sed 's/^v//' )
cp -v ../../../../ios/framework/Info.plist Mapbox.framework
perl -pi -e "s/#####/$VERSION/" Mapbox.framework/Info.plist
perl -pi -e "s/%%%%%/$VERSION/" Mapbox.framework/Info.plist
