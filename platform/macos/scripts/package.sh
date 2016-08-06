#!/usr/bin/env bash

set -e
set -o pipefail
set -u

NAME=Mapbox
OUTPUT=build/macos/pkg
DERIVED_DATA=build/macos
PRODUCTS=${DERIVED_DATA}

BUILDTYPE=${BUILDTYPE:-Release}
GCC_GENERATE_DEBUGGING_SYMBOLS=${SYMBOLS:-YES}

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

rm -rf ${OUTPUT}

HASH=`git log | head -1 | awk '{ print $2 }' | cut -c 1-10` && true
PROJ_VERSION=$(git rev-list --count HEAD)
SEM_VERSION=$( git describe --tags --match=macos-v*.*.* --abbrev=0 | sed 's/^macos-v//' )
SHORT_VERSION=${SEM_VERSION%-*}

step "Building targets (build ${PROJ_VERSION}, version ${SEM_VERSION})…"
xcodebuild \
    GCC_GENERATE_DEBUGGING_SYMBOLS=${GCC_GENERATE_DEBUGGING_SYMBOLS} \
    CURRENT_PROJECT_VERSION=${PROJ_VERSION} \
    CURRENT_SHORT_VERSION=${SHORT_VERSION} \
    CURRENT_SEMANTIC_VERSION=${SEM_VERSION} \
    CURRENT_COMMIT_HASH=${HASH} \
    -derivedDataPath ${DERIVED_DATA} \
    -workspace ./platform/macos/macos.xcworkspace \
    -scheme dynamic \
    -configuration ${BUILDTYPE} \
    -jobs ${JOBS} | xcpretty

step "Copying dynamic framework into place"
mkdir -p "${OUTPUT}/${NAME}.framework"
ditto ${PRODUCTS}/${BUILDTYPE}/${NAME}.framework "${OUTPUT}/${NAME}.framework"
if [[ -e ${PRODUCTS}/${BUILDTYPE}/${NAME}.framework.dSYM ]]; then
    cp -r ${PRODUCTS}/${BUILDTYPE}/${NAME}.framework.dSYM "${OUTPUT}"
fi

if [[ "${GCC_GENERATE_DEBUGGING_SYMBOLS}" == false ]]; then
    step "Stripping binaries…"
    strip -Sx "${OUTPUT}/${NAME}.framework/${NAME}"
fi

step "Copying library resources…"
cp -pv LICENSE.md "${OUTPUT}"
cp -pv platform/macos/docs/pod-README.md "${OUTPUT}/README.md"
sed -n -e '/^## /,$p' platform/macos/CHANGELOG.md > "${OUTPUT}/CHANGELOG.md"

step "Generating API documentation…"
make xdocument OUTPUT="${OUTPUT}/documentation"
