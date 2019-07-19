#!/usr/bin/env bash

set -e
set -o pipefail
set -u

NAME=Mapbox
OUTPUT=build/ios/pkg
DERIVED_DATA=build/ios
PRODUCTS=${DERIVED_DATA}
LOG_PATH=build/xcodebuild-$(date +"%Y-%m-%d_%H%M%S").log

BUILD_FOR_DEVICE=${BUILD_DEVICE:-true}
BUILD_DOCS=${BUILD_DOCS:-true}
SYMBOLS=${SYMBOLS:-YES}

BUILDTYPE=${BUILDTYPE:-Debug}
if [[ ${SYMBOLS} == YES && ${BUILDTYPE} == Release ]]; then
    BUILDTYPE='RelWithDebInfo'
fi

FORMAT=${FORMAT:-dynamic}
BUILD_DYNAMIC=true
BUILD_STATIC=false
if [[ ${FORMAT} == "static" ]]; then
    BUILD_STATIC=true
    BUILD_DYNAMIC=false
elif [[ ${FORMAT} != "dynamic" ]]; then
    echo "Error: FORMAT must be dynamic or static."
    exit 1
fi

SDK=iphonesimulator
if [[ ${BUILD_FOR_DEVICE} == true ]]; then
    SDK=iphoneos
fi
IOS_SDK_VERSION=`xcrun --sdk ${SDK} --show-sdk-version`

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

step "Configuring ${FORMAT} framework for ${SDK} ${IOS_SDK_VERSION} (symbols: ${SYMBOLS}, buildtype: ${BUILDTYPE})"

xcodebuild -version

rm -rf ${OUTPUT}
if [[ ${BUILD_STATIC} == true ]]; then
    mkdir -p "${OUTPUT}"/static
fi
if [[ ${BUILD_DYNAMIC} == true ]]; then
    mkdir -p "${OUTPUT}"/dynamic
fi

step "Recording library version…"
VERSION="${OUTPUT}"/version.txt
echo -n "https://github.com/mapbox/mapbox-gl-native/commit/" > ${VERSION}
HASH=`git log | head -1 | awk '{ print $2 }' | cut -c 1-10` && true
echo -n "mapbox-gl-native "
echo ${HASH}
echo ${HASH} >> ${VERSION}

PROJ_VERSION=$(git rev-list --count HEAD)
SEM_VERSION=$( git describe --tags --match=ios-v*.*.* --abbrev=0 | sed 's/^ios-v//' )
SHORT_VERSION=${SEM_VERSION%-*}

step "Building targets (build ${PROJ_VERSION}, version ${SEM_VERSION})"

SCHEME='dynamic'
if [[ ${BUILD_STATIC} == true ]]; then
    SCHEME='static'
fi

CI_XCCONFIG=''
if [[ ! -z "${CI:=}" ]]; then
    xcconfig='platform/darwin/ci.xcconfig'
    echo "CI environment, using ${xcconfig}"
    CI_XCCONFIG="-xcconfig ./${xcconfig}"
fi

step "Building ${FORMAT} framework for iOS Simulator using ${SCHEME} scheme"
xcodebuild \
    CURRENT_PROJECT_VERSION=${PROJ_VERSION} \
    CURRENT_SHORT_VERSION=${SHORT_VERSION} \
    CURRENT_SEMANTIC_VERSION=${SEM_VERSION} \
    CURRENT_COMMIT_HASH=${HASH} \
    ONLY_ACTIVE_ARCH=NO \
    ${CI_XCCONFIG} \
    -derivedDataPath ${DERIVED_DATA} \
    -workspace ./platform/ios/ios.xcworkspace \
    -scheme ${SCHEME} \
    -configuration ${BUILDTYPE} \
    -sdk iphonesimulator \
    -jobs ${JOBS} | tee ${LOG_PATH} | xcpretty

if [[ ${BUILD_FOR_DEVICE} == true ]]; then
    step "Building ${FORMAT} framework for iOS devices using ${SCHEME} scheme"
    xcodebuild \
        CURRENT_PROJECT_VERSION=${PROJ_VERSION} \
        CURRENT_SHORT_VERSION=${SHORT_VERSION} \
        CURRENT_SEMANTIC_VERSION=${SEM_VERSION} \
        CURRENT_COMMIT_HASH=${HASH} \
        ONLY_ACTIVE_ARCH=NO \
        ${CI_XCCONFIG} \
        -derivedDataPath ${DERIVED_DATA} \
        -workspace ./platform/ios/ios.xcworkspace \
        -scheme ${SCHEME} \
        -configuration ${BUILDTYPE} \
        -sdk iphoneos \
        -jobs ${JOBS} | tee ${LOG_PATH} | xcpretty
fi

LIBS=(Mapbox.a)

# https://medium.com/@syshen/create-an-ios-universal-framework-148eb130a46c
if [[ ${BUILD_FOR_DEVICE} == true ]]; then
    if [[ ${BUILD_STATIC} == true ]]; then
        step "Assembling static framework for iOS Simulator and devices…"
        mkdir -p ${OUTPUT}/static/${NAME}.framework
        libtool -static -no_warning_for_no_symbols \
            -o ${OUTPUT}/static/${NAME}.framework/${NAME} \
            ${LIBS[@]/#/${PRODUCTS}/${BUILDTYPE}-iphoneos/lib} \
            ${LIBS[@]/#/${PRODUCTS}/${BUILDTYPE}-iphonesimulator/lib} \
            `cmake -LA -N ${DERIVED_DATA} | grep MASON_PACKAGE_icu_LIBRARIES | cut -d= -f2`

        cp -rv ${PRODUCTS}/${BUILDTYPE}-iphoneos/${NAME}.bundle ${OUTPUT}/static
    fi

    if [[ ${BUILD_DYNAMIC} == true ]]; then
        step "Copying dynamic framework into place for iOS devices"
        cp -r \
            ${PRODUCTS}/${BUILDTYPE}-iphoneos/${NAME}.framework \
            ${OUTPUT}/dynamic/

        if [[ -e ${PRODUCTS}/${BUILDTYPE}-iphoneos/${NAME}.framework.dSYM ]]; then
            step "Copying dSYM"
            cp -r ${PRODUCTS}/${BUILDTYPE}-iphoneos/${NAME}.framework.dSYM \
                  ${OUTPUT}/dynamic/
            if [[ -e ${PRODUCTS}/${BUILDTYPE}-iphonesimulator/${NAME}.framework.dSYM ]]; then
                step "Merging device and simulator dSYMs…"
                lipo \
                    ${PRODUCTS}/${BUILDTYPE}-iphoneos/${NAME}.framework.dSYM/Contents/Resources/DWARF/${NAME} \
                    ${PRODUCTS}/${BUILDTYPE}-iphonesimulator/${NAME}.framework.dSYM/Contents/Resources/DWARF/${NAME} \
                    -create -output ${OUTPUT}/dynamic/${NAME}.framework.dSYM/Contents/Resources/DWARF/${NAME}
                lipo -info ${OUTPUT}/dynamic/${NAME}.framework.dSYM/Contents/Resources/DWARF/${NAME}
            fi
        fi

        step "Merging simulator dynamic library into device dynamic library…"
        lipo \
            ${PRODUCTS}/${BUILDTYPE}-iphoneos/${NAME}.framework/${NAME} \
            ${PRODUCTS}/${BUILDTYPE}-iphonesimulator/${NAME}.framework/${NAME} \
            -create -output ${OUTPUT}/dynamic/${NAME}.framework/${NAME} | echo
    fi
    
    cp -rv platform/ios/app/Settings.bundle ${OUTPUT}
else
    if [[ ${BUILD_STATIC} == true ]]; then
        step "Assembling static library for iOS Simulator…"
        mkdir -p ${OUTPUT}/static/${NAME}.framework
        libtool -static -no_warning_for_no_symbols \
            -o ${OUTPUT}/static/${NAME}.framework/${NAME} \
            ${LIBS[@]/#/${PRODUCTS}/${BUILDTYPE}-iphonesimulator/lib} \
            `cmake -LA -N ${DERIVED_DATA} | grep MASON_PACKAGE_icu_LIBRARIES | cut -d= -f2`

        cp -rv ${PRODUCTS}/${BUILDTYPE}-iphonesimulator/${NAME}.bundle ${OUTPUT}/static
    fi

    if [[ ${BUILD_DYNAMIC} == true ]]; then
        step "Copying dynamic framework into place for iOS Simulator…"
        cp -r \
            ${PRODUCTS}/${BUILDTYPE}-iphonesimulator/${NAME}.framework \
            ${OUTPUT}/dynamic/${NAME}.framework
        if [[ -e ${PRODUCTS}/${BUILDTYPE}-iphonesimulator/${NAME}.framework.dSYM ]]; then
            step "Copying dSYM"
            cp -r ${PRODUCTS}/${BUILDTYPE}-iphonesimulator/${NAME}.framework.dSYM \
                ${OUTPUT}/dynamic/
        fi
    fi
    
    cp -rv platform/ios/app/Settings.bundle ${OUTPUT}
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

if [[ ${BUILD_DYNAMIC} == true && ${BUILDTYPE} == Release ]]; then
    validate_dsym \
        "${OUTPUT}/dynamic/${NAME}.framework.dSYM/Contents/Resources/DWARF/${NAME}" \
        "${OUTPUT}/dynamic/${NAME}.framework/${NAME}"

        step "Removing i386 slice from dSYM"
        lipo -remove i386 "${OUTPUT}/dynamic/${NAME}.framework.dSYM/Contents/Resources/DWARF/${NAME}" -o "${OUTPUT}/dynamic/${NAME}.framework.dSYM/Contents/Resources/DWARF/${NAME}"
        lipo -info "${OUTPUT}/dynamic/${NAME}.framework.dSYM/Contents/Resources/DWARF/${NAME}"
fi

if [[ ${BUILD_STATIC} == true ]]; then
    step "Copying static library headers…"
    cp -rv "${PRODUCTS}/${BUILDTYPE}-iphoneos/Headers" "${OUTPUT}/static/${NAME}.framework/Headers"
    cat platform/ios/framework/Mapbox-static.h > "${OUTPUT}/static/${NAME}.framework/Headers/Mapbox.h"
    cat "${PRODUCTS}/${BUILDTYPE}-iphoneos/Headers/Mapbox.h" >> "${OUTPUT}/static/${NAME}.framework/Headers/Mapbox.h"
fi

step "Copying library resources…"
cp -pv LICENSE.md ${OUTPUT}
if [[ ${BUILD_STATIC} == true ]]; then
    cp -pv "${OUTPUT}/static/${NAME}.bundle/Info.plist" "${OUTPUT}/static/${NAME}.framework/Info.plist"
    plutil -replace CFBundlePackageType -string FMWK "${OUTPUT}/static/${NAME}.framework/Info.plist"
    mkdir "${OUTPUT}/static/${NAME}.framework/Modules"
    cp -pv platform/ios/framework/modulemap "${OUTPUT}/static/${NAME}.framework/Modules/module.modulemap"
fi
if [[ ${BUILD_DYNAMIC} == true && ${BUILD_FOR_DEVICE} == true ]]; then
    step "Copying bitcode symbol maps…"
    find "${PRODUCTS}/${BUILDTYPE}-iphoneos" -name '*.bcsymbolmap' -type f -exec cp -pv {} "${OUTPUT}/dynamic/" \;
fi
sed -n -e '/^## /,$p' platform/ios/CHANGELOG.md > "${OUTPUT}/CHANGELOG.md"

rm -rf /tmp/mbgl
mkdir -p /tmp/mbgl/
README=/tmp/mbgl/README.md
cp platform/ios/docs/pod-README.md "${README}"
if [[ ${BUILD_DYNAMIC} == false ]]; then
    sed -i '' -e '/{{DYNAMIC}}/,/{{\/DYNAMIC}}/d' "${README}"
fi
if [[ ${BUILD_STATIC} == false ]]; then
    sed -i '' -e '/{{STATIC}}/,/{{\/STATIC}}/d' "${README}"
fi
sed -i '' \
    -e '/{{DYNAMIC}}/d' -e '/{{\/DYNAMIC}}/d' \
    -e '/{{STATIC}}/d' -e '/{{\/STATIC}}/d' \
    "${README}"
cp ${README} "${OUTPUT}"

if [ ${BUILD_DOCS} == true ]; then
    step "Generating API documentation for ${BUILDTYPE} Build…"
    make idocument OUTPUT="${OUTPUT}/documentation"
fi
