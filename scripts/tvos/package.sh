#!/usr/bin/env bash

set -e
set -o pipefail
set -u

NAME=Mapbox
OUTPUT=build/tvos/pkg
TV_SDK_VERSION=`xcrun --sdk appletvos --show-sdk-version`
# Need this for mason packages, because tvos versions haven't been added.
# IOS_SDK_VERSION=`xcrun --sdk iphoneos --show-sdk-version`
LIBUV_VERSION=0.10.28

if [[ ${#} -eq 0 ]]; then # e.g. "make tvpackage"
    BUILD_FOR_DEVICE=true
    GCC_GENERATE_DEBUGGING_SYMBOLS="YES"
elif [[ ${1} == "sim" ]]; then # e.g. "make tvpackage-sim"
    BUILD_FOR_DEVICE=false
    GCC_GENERATE_DEBUGGING_SYMBOLS="YES"
else # e.g. "make tvpackage-strip"
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
export MASON_PLATFORM=tvos
export BUILDTYPE=${BUILDTYPE:-Release}
export HOST=tvos

if [[ "${BUILD_FOR_DEVICE}" == true ]]; then
    step "Building tvOS device targets..."
    xcodebuild -sdk appletvos${TV_SDK_VERSION} \
        ARCHS="arm64" \
        ONLY_ACTIVE_ARCH=NO \
        GCC_GENERATE_DEBUGGING_SYMBOLS=${GCC_GENERATE_DEBUGGING_SYMBOLS} \
        -project ./build/tvos-all/mbgl.xcodeproj \
        -configuration ${BUILDTYPE} \
        -target everything \
        -jobs ${JOBS} | xcpretty -c
fi

step "Building tvOS Simulator targets..."
xcodebuild -sdk appletvsimulator${TV_SDK_VERSION} \
    ARCHS="x86_64 i386" \
    ONLY_ACTIVE_ARCH=NO \
    GCC_GENERATE_DEBUGGING_SYMBOLS=${GCC_GENERATE_DEBUGGING_SYMBOLS} \
    -project ./build/tvos-all/mbgl.xcodeproj \
    -configuration ${BUILDTYPE} \
    -target everything \
    -jobs ${JOBS} | xcpretty -c


step "Building static library..."
LIBS=(core.a platform-tvos.a asset-fs.a cache-sqlite.a http-nsurl.a)
if [[ "${BUILD_FOR_DEVICE}" == true ]]; then
    echo \
    libtool -static -no_warning_for_no_symbols \
        -o ${OUTPUT}/static/lib${NAME}.a \
        ${LIBS[@]/#/build/${BUILDTYPE}-appletvos/libmbgl-} \
        ${LIBS[@]/#/build/${BUILDTYPE}-appletvsimulator/libmbgl-} \
        `find mason_packages/tvos-${TV_SDK_VERSION} -type f -name libuv.a` \
        `find mason_packages/tvos-${TV_SDK_VERSION} -type f -name libgeojsonvt.a`

    libtool -static -no_warning_for_no_symbols \
        -o ${OUTPUT}/static/lib${NAME}.a \
        ${LIBS[@]/#/build/${BUILDTYPE}-appletvos/libmbgl-} \
        ${LIBS[@]/#/build/${BUILDTYPE}-appletvsimulator/libmbgl-} \
        `find mason_packages/tvos-${TV_SDK_VERSION} -type f -name libuv.a` \
        `find mason_packages/tvos-${TV_SDK_VERSION} -type f -name libgeojsonvt.a`
else
    libtool -static -no_warning_for_no_symbols \
        -o ${OUTPUT}/static/lib${NAME}.a \
        ${LIBS[@]/#/build/${BUILDTYPE}-appletvsimulator/libmbgl-} \
        `find mason_packages/tvos-${TV_SDK_VERSION} -type f -name libuv.a` \
        `find mason_packages/tvos-${TV_SDK_VERSION} -type f -name libgeojsonvt.a`
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
mkdir -p "${OUTPUT}/static/${NAME}.bundle/styles"
cp -pv styles/styles/{dark,emerald,light,streets,satellite}-v8.json "${OUTPUT}/static/${NAME}.bundle/styles"

step "Creating API Docs..."
if [ -z `which appledoc` ]; then
    echo "Unable to find appledoc. See https://github.com/mapbox/mapbox-gl-native#manually"
    exit 1
fi
DOCS_OUTPUT="${OUTPUT}/static/Docs"
git fetch --tags
DOCS_VERSION=$( git tag | sed 's/^ios-//' | sort -r | grep -v '\-rc.' | grep -v '\-pre.' | sed -n '1p' | sed 's/^v//' )
rm -rf /tmp/mbgl
mkdir -p /tmp/mbgl/
README=/tmp/mbgl/README.md
cat ios/docs/pod-README.md > ${README}
echo >> ${README}
echo -n "#" >> ${README}
cat CHANGELOG.md >> ${README}
# Copy headers to a temporary location where we can substitute macros that appledoc doesn't understand.
cp -r "${OUTPUT}/static/Headers" /tmp/mbgl
perl \
    -pi \
    -e 's/NS_(?:(MUTABLE)_)?(ARRAY|SET|DICTIONARY)_OF\(\s*(.+?)\s*\)/NS\L\u$1\u$2\E <$3>/g' \
    /tmp/mbgl/Headers/*.h
appledoc \
    --output ${DOCS_OUTPUT} \
    --project-name "Mapbox iOS SDK ${DOCS_VERSION}" \
    --project-company Mapbox \
    --create-html \
    --no-create-docset \
    --no-install-docset \
    --company-id com.mapbox \
    --index-desc ${README} \
    /tmp/mbgl/Headers
cp ${README} "${OUTPUT}/static"
