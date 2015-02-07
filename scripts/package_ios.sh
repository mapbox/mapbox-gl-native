#!/usr/bin/env bash

set -e
set -o pipefail
set -u

NAME=MapboxGL
OUTPUT=build/ios/pkg
IOS_SDK_VERSION=8.1
LIBUV_VERSION=0.10.28

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT


rm -rf ${OUTPUT}
mkdir -p "${OUTPUT}"/static


step "Recording library version..."
VERSION="${OUTPUT}"/static/version.txt
echo -n "https://github.com/mapbox/mapbox-gl-native/commit/" > ${VERSION}
HASH=`git log | head -1 | awk '{ print $2 }' | cut -c 1-10` && true
echo -n "mapbox-gl-native "
echo ${HASH}
echo ${HASH} >> ${VERSION}


step "Creating build files..."
export MASON_PLATFORM=ios
export BUILDTYPE=${BUILDTYPE:-Release}
export HOST=ios
make Xcode/mbgl

step "Building iOS targets..."
xcodebuild -sdk iphoneos${IOS_SDK_VERSION} \
    ARCHS="arm64 armv7 armv7s" \
    -project ./build/ios/mbgl.xcodeproj \
    -configuration ${BUILDTYPE} \
    -target everything \
    -jobs `sysctl -n hw.ncpu` | xcpretty -c


step "Building iOS Simulator targets..."
xcodebuild -sdk iphonesimulator${IOS_SDK_VERSION} \
    ARCHS="x86_64 i386" \
    -project ./build/ios/mbgl.xcodeproj \
    -configuration ${BUILDTYPE} \
    -target everything \
    -jobs `sysctl -n hw.ncpu` | xcpretty -c


step "Building static library..."
LIBS=(core.a platform-ios.a asset-fs.a cache-sqlite.a http-nsurl.a)
libtool -static -no_warning_for_no_symbols \
    -o ${OUTPUT}/static/lib${NAME}.a \
    ${LIBS[@]/#/build/${BUILDTYPE}-iphoneos/libmbgl-} \
    ${LIBS[@]/#/build/${BUILDTYPE}-iphonesimulator/libmbgl-} \
    `find mason_packages/ios-${IOS_SDK_VERSION} -type f -name libuv.a`
echo "Created ${OUTPUT}/static/lib${NAME}.a"


step "Copying Headers..."
mkdir -p "${OUTPUT}/static/Headers"
cp -pv include/mbgl/ios/* "${OUTPUT}/static/Headers"


# Manually create resource bundle. We don't use a GYP target here because of
# complications between faked GYP bundles-as-executables, device build
# dependencies, and code signing.
step "Copying Resources..."
mkdir -p "${OUTPUT}/static/${NAME}.bundle"
cp -pv platform/ios/resources/* "${OUTPUT}/static/${NAME}.bundle"
cp -prv styles/styles "${OUTPUT}/static/${NAME}.bundle/styles"
