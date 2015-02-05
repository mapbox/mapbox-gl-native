#!/usr/bin/env bash

set -e
set -o pipefail
set -u

NAME=MapboxGL
OUTPUT=build/ios/pkg
LIBUV_VERSION=0.10.28

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT


rm -rf ${OUTPUT}
mkdir -p "${OUTPUT}"/{static,dynamic}


step "Creating build files..."
export MASON_PLATFORM=ios
export BUILDTYPE=${BUILDTYPE:-Release}
export HOST=ios
make Xcode/mbgl

step "Build iOS targets..."
xcodebuild -sdk iphoneos ARCHS="arm64 armv7 armv7s" -project ./build/ios/mbgl.xcodeproj -configuration ${BUILDTYPE} -target everything -jobs `sysctl -n hw.ncpu` | xcpretty -c


step "Build iOS Simulator targets..."
xcodebuild -sdk iphonesimulator ARCHS="x86_64 i386" -project ./build/ios/mbgl.xcodeproj -configuration ${BUILDTYPE} -target everything -jobs `sysctl -n hw.ncpu` | xcpretty -c


step "Build static library..."
LIBS=(core.a platform-ios.a asset-fs.a cache-sqlite.a http-nsurl.a)
libtool -static -no_warning_for_no_symbols \
    -o ${OUTPUT}/static/lib${NAME}.a \
    ${LIBS[@]/#/build/${BUILDTYPE}-iphoneos/libmbgl-} \
    ${LIBS[@]/#/build/${BUILDTYPE}-iphonesimulator/libmbgl-} \
    `.mason/mason static_libs libuv ${LIBUV_VERSION}`
echo "Created ${OUTPUT}/static/lib${NAME}.a"


step "Copying Headers..."
mkdir -p "${OUTPUT}/static/Headers"
cp -pv include/mbgl/ios/* "${OUTPUT}/static/Headers"


# Manually create resource bundle. We don't use a GYP target here because of
# complications between faked GYP bundles-as-executables, device build
# dependencies, and code signing.
step "Copy Resources..."
mkdir -p "${OUTPUT}/static/${NAME}.bundle"
cp -pv platform/ios/resources/* "${OUTPUT}/static/${NAME}.bundle"
cp -prv styles/styles "${OUTPUT}/static/${NAME}.bundle/styles"


step "Copy Readme and FAQ..."
cp -pv platform/ios/*.md "${OUTPUT}/"
