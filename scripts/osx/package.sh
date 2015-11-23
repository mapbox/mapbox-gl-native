#!/usr/bin/env bash

set -e
set -o pipefail
set -u

NAME=Mapbox
OUTPUT=build/osx/pkg
OSX_SDK_VERSION=`xcrun --sdk macosx --show-sdk-version`
LIBUV_VERSION=1.7.5

if [[ ${#} -eq 0 ]]; then # e.g. "make ipackage"
    BUILDTYPE="Release"
    GCC_GENERATE_DEBUGGING_SYMBOLS="YES"
else # e.g. "make ipackage-strip"
    BUILDTYPE="Release"
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
export MASON_PLATFORM=osx
export BUILDTYPE=${BUILDTYPE:-Release}
export HOST=osx
make Xcode/osx

step "Building OS X targets..."
xcodebuild -sdk macosx${OSX_SDK_VERSION} \
    ARCHS="x86_64" \
    ONLY_ACTIVE_ARCH=NO \
    GCC_GENERATE_DEBUGGING_SYMBOLS=${GCC_GENERATE_DEBUGGING_SYMBOLS} \
    -project ./build/osx-x86_64/gyp/mbgl.xcodeproj \
    -configuration ${BUILDTYPE} \
    -target everything \
    -jobs ${JOBS}


step "Building static library..."
LIBS=(core.a platform-osx.a asset-fs.a cache-sqlite.a http-nsurl.a)
libtool -static -no_warning_for_no_symbols \
    `find mason_packages/osx-${OSX_SDK_VERSION} -type f -name libuv.a` \
    `find mason_packages/osx-${OSX_SDK_VERSION} -type f -name libgeojsonvt.a` \
    -o ${OUTPUT}/static/lib${NAME}.a \
    ${LIBS[@]/#/gyp/build/${BUILDTYPE}/libmbgl-}
echo "Created ${OUTPUT}/static/lib${NAME}.a"


step "Copying Headers..."
mkdir -p "${OUTPUT}/static/Headers"
for i in `ls -R include/mbgl/darwin | grep -vi private`; do
    cp -pv include/mbgl/darwin/$i "${OUTPUT}/static/Headers"
done
for i in `ls -R include/mbgl/osx | grep -vi private`; do
    cp -pv include/mbgl/osx/$i "${OUTPUT}/static/Headers"
done

step "Copying Resources..."
cp -pv LICENSE.md "${OUTPUT}/static"
mkdir -p "${OUTPUT}/static/${NAME}.bundle"
cp -pv platform/osx/resources/* "${OUTPUT}/static/${NAME}.bundle"

