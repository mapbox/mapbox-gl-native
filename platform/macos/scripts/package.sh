#!/usr/bin/env bash

set -e
set -o pipefail
set -u

NAME=Mapbox
OUTPUT=build/macos/pkg
APP_OUTPUT=build/macos/app
DERIVED_DATA=build/macos

BUILDTYPE=${BUILDTYPE:-Release}
SYMBOLS=${SYMBOLS:-YES}
PRODUCTS=${DERIVED_DATA}/${BUILDTYPE}

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

rm -rf ${OUTPUT} ${APP_OUTPUT}

HASH=`git log | head -1 | awk '{ print $2 }' | cut -c 1-10` && true
PROJ_VERSION=$(git rev-list --count HEAD)
SEM_VERSION=$( git describe --tags --match=macos-v*.*.* --abbrev=0 | sed 's/^macos-v//' )
SHORT_VERSION=${SEM_VERSION%-*}

CI_XCCONFIG=''
if [[ ! -z "${CI:=}" ]]; then
    xcconfig='platform/darwin/ci.xcconfig'
    echo "CI environment, using ${xcconfig}"
    CI_XCCONFIG="-xcconfig ./${xcconfig}"
fi

step "Building dynamic framework (build ${PROJ_VERSION}, version ${SEM_VERSION})…"
xcodebuild \
    CURRENT_PROJECT_VERSION=${PROJ_VERSION} \
    CURRENT_SHORT_VERSION=${SHORT_VERSION} \
    CURRENT_SEMANTIC_VERSION=${SEM_VERSION} \
    CURRENT_COMMIT_HASH=${HASH} \
    ${CI_XCCONFIG} \
    -derivedDataPath ${DERIVED_DATA} \
    -archivePath "${APP_OUTPUT}/macosapp.xcarchive" \
    -workspace ./platform/macos/macos.xcworkspace \
    -scheme dynamic \
    -configuration ${BUILDTYPE} \
    -jobs ${JOBS} \
    build | xcpretty

step "Copying dynamic framework into place"
mkdir -p "${OUTPUT}/${NAME}.framework"
ditto ${PRODUCTS}/${NAME}.framework "${OUTPUT}/${NAME}.framework"
if [[ -e ${PRODUCTS}/${NAME}.framework.dSYM ]]; then
    cp -r ${PRODUCTS}/${NAME}.framework.dSYM "${OUTPUT}"
fi

step "Building and archiving Mapbox GL.app (build ${PROJ_VERSION}, version ${SEM_VERSION})…"
if [[ ${BUILDTYPE} == Release ]]; then
    mkdir -p ${APP_OUTPUT}
    xcodebuild \
        CURRENT_PROJECT_VERSION=${PROJ_VERSION} \
        CURRENT_SHORT_VERSION=${SHORT_VERSION} \
        CURRENT_SEMANTIC_VERSION=${SEM_VERSION} \
        CURRENT_COMMIT_HASH=${HASH} \
        ${CI_XCCONFIG} \
        -derivedDataPath ${DERIVED_DATA} \
        -archivePath "${APP_OUTPUT}/macosapp.xcarchive" \
        -workspace ./platform/macos/macos.xcworkspace \
        -scheme macosapp \
        -configuration ${BUILDTYPE} \
        -jobs ${JOBS} \
        archive | xcpretty
fi

if [[ ${SYMBOLS} = NO ]]; then
    step "Stripping symbols from binaries"
    strip -Sx "${OUTPUT}/${NAME}.framework/${NAME}"
fi

function get_comparable_uuid {
    echo $(dwarfdump --uuid ${1} | sed -n 's/.*UUID:\([^\"]*\) .*/\1/p' | sort)
}

function validate_dsym {
    step "Validating dSYM and framework UUIDs…"
    DSYM_UUID=$(get_comparable_uuid "${1}")
    FRAMEWORK_UUID=$(get_comparable_uuid "${2}")
    echo -e "${1}\n  ${DSYM_UUID}\n${2}\n  ${FRAMEWORK_UUID}"
    if [[ ${DSYM_UUID} != ${FRAMEWORK_UUID} ]]; then
        echo "Error: dSYM and framework UUIDs do not match."
        exit 1
    fi
}

if [[ ${BUILDTYPE} == Release ]]; then
    validate_dsym \
        "${OUTPUT}/${NAME}.framework.dSYM/Contents/Resources/DWARF/${NAME}" \
        "${OUTPUT}/${NAME}.framework/${NAME}"
    
    step "Exporting Mapbox GL.app"
    xcodebuild \
        ${CI_XCCONFIG} \
        -exportArchive \
        -archivePath "${APP_OUTPUT}/macosapp.xcarchive" \
        -exportPath "${APP_OUTPUT}" \
        -exportOptionsPlist platform/macos/ExportOptions.plist
fi

function create_podspec {
    step "Creating local podspec…"
    [[ $SYMBOLS = YES ]] && POD_SUFFIX="-symbols" || POD_SUFFIX=""
    POD_SOURCE_PATH='    :path => ".",'
    POD_FRAMEWORKS="  m.vendored_frameworks = '"${NAME}".framework'"
    INPUT_PODSPEC=platform/macos/${NAME}-macOS-SDK${POD_SUFFIX}.podspec
    OUTPUT_PODSPEC=${OUTPUT}/${NAME}-macOS-SDK${POD_SUFFIX}.podspec
    sed "s/.*:http.*/${POD_SOURCE_PATH}/" ${INPUT_PODSPEC} > ${OUTPUT_PODSPEC}
    sed -i '' "s/.*vendored_frameworks.*/${POD_FRAMEWORKS}/" ${OUTPUT_PODSPEC}
}

create_podspec

step "Copying library resources…"
cp -pv LICENSE.md "${OUTPUT}"
cp -pv platform/macos/docs/pod-README.md "${OUTPUT}/README.md"
sed -n -e '/^## /,$p' platform/macos/CHANGELOG.md > "${OUTPUT}/CHANGELOG.md"

step "Generating API documentation…"
make xdocument OUTPUT="${OUTPUT}/documentation"

step "Checking that all public symbols are exported…"
node platform/darwin/scripts/check-public-symbols.js macOS
