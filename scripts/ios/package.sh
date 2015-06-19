#!/usr/bin/env bash

set -e
set -o pipefail
set -u

NAME=MapboxGL
OUTPUT=build/ios/pkg
IOS_SDK_VERSION=`xcrun --sdk iphoneos --show-sdk-version`
LIBUV_VERSION=0.10.28

if [[ ${#} -eq 0 ]]; then # e.g. "make ipackage"
    BUILD_FOR_DEVICE=true
    GCC_GENERATE_DEBUGGING_SYMBOLS="YES"
elif [[ ${1} == "sim" ]]; then # e.g. "make ipackage-sim"
    BUILD_FOR_DEVICE=false
    GCC_GENERATE_DEBUGGING_SYMBOLS="YES"
else # e.g. "make ipackage-strip"
    BUILD_FOR_DEVICE=true
    GCC_GENERATE_DEBUGGING_SYMBOLS="NO"
fi

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
make Xcode/ios

if [[ "${BUILD_FOR_DEVICE}" == true ]]; then
    step "Building iOS device targets..."
    xcodebuild -sdk iphoneos${IOS_SDK_VERSION} \
        ARCHS="arm64 armv7 armv7s" \
        ONLY_ACTIVE_ARCH=NO \
        GCC_GENERATE_DEBUGGING_SYMBOLS=${GCC_GENERATE_DEBUGGING_SYMBOLS} \
        -project ./build/ios/mbgl.xcodeproj \
        -configuration ${BUILDTYPE} \
        -target everything \
        -jobs ${JOBS} | xcpretty -c
fi

step "Building iOS Simulator targets..."
xcodebuild -sdk iphonesimulator${IOS_SDK_VERSION} \
    ARCHS="x86_64 i386" \
    ONLY_ACTIVE_ARCH=NO \
    GCC_GENERATE_DEBUGGING_SYMBOLS=${GCC_GENERATE_DEBUGGING_SYMBOLS} \
    -project ./build/ios/mbgl.xcodeproj \
    -configuration ${BUILDTYPE} \
    -target everything \
    -jobs ${JOBS} | xcpretty -c


step "Building static library..."
LIBS=(core.a platform-ios.a asset-fs.a cache-sqlite.a http-nsurl.a)
if [[ "${BUILD_FOR_DEVICE}" == true ]]; then
    libtool -static -no_warning_for_no_symbols \
        -o ${OUTPUT}/static/lib${NAME}.a \
        ${LIBS[@]/#/build/${BUILDTYPE}-iphoneos/libmbgl-} \
        ${LIBS[@]/#/build/${BUILDTYPE}-iphonesimulator/libmbgl-} \
        `find mason_packages/ios-${IOS_SDK_VERSION} -type f -name libuv.a`
else
    libtool -static -no_warning_for_no_symbols \
        -o ${OUTPUT}/static/lib${NAME}.a \
        ${LIBS[@]/#/build/${BUILDTYPE}-iphonesimulator/libmbgl-} \
        `find mason_packages/ios-${IOS_SDK_VERSION} -type f -name libuv.a`
fi
echo "Created ${OUTPUT}/static/lib${NAME}.a"


step "Copying Headers..."
mkdir -p "${OUTPUT}/static/Headers"
for i in `ls -R include/mbgl/ios | grep -vi private`; do
    cp -pv include/mbgl/ios/$i "${OUTPUT}/static/Headers"
done

step "Setting up dummy source file for CocoaPods 0.37.0..."
echo "// https://github.com/mapbox/mapbox-gl-native/issues/1426" > "${OUTPUT}/static/MGLDummy.m"


# Manually create resource bundle. We don't use a GYP target here because of
# complications between faked GYP bundles-as-executables, device build
# dependencies, and code signing.
step "Copying Resources..."
cp -pv LICENSE.md "${OUTPUT}/static"
mkdir -p "${OUTPUT}/static/${NAME}.bundle"
cp -pv platform/ios/resources/* "${OUTPUT}/static/${NAME}.bundle"
cp -prv styles/styles "${OUTPUT}/static/${NAME}.bundle/styles"

step "Creating API Docs..."
if [ -z `which appledoc` ]; then
    echo "Unable to find appledoc. See https://github.com/mapbox/mapbox-gl-native#manually"
    exit 1
fi
DOCS_OUTPUT="${OUTPUT}/static/Docs"
DOCS_VERSION=$( git tag --sort -v:refname | grep -v '\-rc.' | sed -n '1p' | sed 's/^v//' )
README="/tmp/GL-README.md"
cat ios/README.md > ${README}
echo >> ${README}
echo -n "#" >> ${README}
cat CHANGELOG.md >> ${README}
appledoc \
    --output ${DOCS_OUTPUT} \
    --project-name "Mapbox GL for iOS ${DOCS_VERSION}" \
    --project-company Mapbox \
    --create-html \
    --no-create-docset \
    --no-install-docset \
    --company-id com.mapbox \
    --ignore include/mbgl/ios/private \
    --index-desc ${README} \
    include/mbgl/ios
