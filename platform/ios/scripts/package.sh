#!/usr/bin/env bash

set -e
set -o pipefail
set -u

NAME=Mapbox
OUTPUT=build/ios/pkg
DERIVED_DATA=build/ios
PRODUCTS=${DERIVED_DATA}/Build/Products

BUILDTYPE=${BUILDTYPE:-Release}
BUILD_FOR_DEVICE=${BUILD_DEVICE:-true}
GCC_GENERATE_DEBUGGING_SYMBOLS=${SYMBOLS:-YES}

BUILD_DYNAMIC=true
BUILD_STATIC=true
if [[ ${FORMAT} == "static" ]]; then
    BUILD_DYNAMIC=false
elif [[ ${FORMAT} == "dynamic" ]]; then
    BUILD_STATIC=false
fi

BUNDLE_RESOURCES=${BUNDLE_RESOURCES:-}
BUNDLE_PATH=
if [[ ${BUNDLE_RESOURCES} ]]; then
    BUNDLE_PATH="/${NAME}.bundle"
fi

PLACE_RESOURCE_BUNDLES_OUTSIDE_FRAMEWORK=${PLACE_RESOURCE_BUNDLES_OUTSIDE_FRAMEWORK:-}
STATIC_BUNDLE_PATH=
if [[ ${PLACE_RESOURCE_BUNDLES_OUTSIDE_FRAMEWORK} ]]; then
    STATIC_BUNDLE_PATH="${OUTPUT}/static${BUNDLE_PATH}"
else
    STATIC_BUNDLE_PATH="${OUTPUT}/static/${NAME}.framework${BUNDLE_PATH}"
fi

STATIC_SETTINGS_DIRECTORY=
if [[ ${PLACE_RESOURCE_BUNDLES_OUTSIDE_FRAMEWORK} ]]; then
    STATIC_SETTINGS_DIRECTORY="${OUTPUT}"
else
    STATIC_SETTINGS_DIRECTORY="${OUTPUT}/static/${NAME}.framework"
fi

SDK=iphonesimulator
if [[ ${BUILD_FOR_DEVICE} == true ]]; then
    SDK=iphoneos
fi
IOS_SDK_VERSION=`xcrun --sdk ${SDK} --show-sdk-version`

echo "Configuring ${FORMAT:-dynamic and static} ${BUILDTYPE} framework for ${SDK}; symbols: ${GCC_GENERATE_DEBUGGING_SYMBOLS}; Mapbox.bundle: ${BUNDLE_RESOURCES} bundle.outside: ${PLACE_RESOURCE_BUNDLES_OUTSIDE_FRAMEWORK}"

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT


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

step "Building targets (build ${PROJ_VERSION})…"

SCHEME='dynamic'
if [[ ${BUILD_DYNAMIC} == true && ${BUILD_STATIC} == true ]]; then
    SCHEME+='+static'
elif [[ ${BUILD_STATIC} == true ]]; then
    SCHEME='static'
fi

xcodebuild \
    GCC_GENERATE_DEBUGGING_SYMBOLS=${GCC_GENERATE_DEBUGGING_SYMBOLS} \
    CURRENT_PROJECT_VERSION=${PROJ_VERSION} \
    -derivedDataPath ${DERIVED_DATA} \
    -workspace ./platform/ios/ios.xcworkspace \
    -scheme ${SCHEME} \
    -configuration ${BUILDTYPE} \
    -sdk iphonesimulator \
    -jobs ${JOBS} | xcpretty

if [[ ${BUILD_FOR_DEVICE} == true ]]; then
    xcodebuild \
        GCC_GENERATE_DEBUGGING_SYMBOLS=${GCC_GENERATE_DEBUGGING_SYMBOLS} \
        CURRENT_PROJECT_VERSION=${PROJ_VERSION} \
        -derivedDataPath ${DERIVED_DATA} \
        -workspace ./platform/ios/ios.xcworkspace \
        -scheme ${SCHEME} \
        -configuration ${BUILDTYPE} \
        -sdk iphoneos \
        -jobs ${JOBS} | xcpretty
fi

LIBS=(core.a platform-ios.a)

# https://medium.com/@syshen/create-an-ios-universal-framework-148eb130a46c
if [[ "${BUILD_FOR_DEVICE}" == true ]]; then
    if [[ ${BUILD_STATIC} == true ]]; then
        step "Assembling static framework for iOS Simulator and devices…"
        mkdir -p ${OUTPUT}/static/${NAME}.framework
        libtool -static -no_warning_for_no_symbols \
            `find mason_packages/ios-${IOS_SDK_VERSION} -type f -name libgeojsonvt.a` \
            -o ${OUTPUT}/static/${NAME}.framework/${NAME} \
            ${LIBS[@]/#/${PRODUCTS}/${BUILDTYPE}-iphoneos/libmbgl-} \
            ${LIBS[@]/#/${PRODUCTS}/${BUILDTYPE}-iphonesimulator/libmbgl-}
    fi

    if [[ ${BUILD_DYNAMIC} == true ]]; then
        step "Copying dynamic framework into place for iOS devices"
        cp -r \
            ${PRODUCTS}/${BUILDTYPE}-iphoneos/${NAME}.framework \
            ${OUTPUT}/dynamic/
        if [[ -e ${PRODUCTS}/${BUILDTYPE}-iphoneos/${NAME}.framework.dSYM ]]; then
            cp -r ${PRODUCTS}/${BUILDTYPE}-iphoneos/${NAME}.framework.dSYM \
                ${OUTPUT}/dynamic/
        fi

        step "Merging simulator dynamic library into device dynamic library…"
        lipo \
            ${PRODUCTS}/${BUILDTYPE}-iphoneos/${NAME}.framework/${NAME} \
            ${PRODUCTS}/${BUILDTYPE}-iphonesimulator/${NAME}.framework/${NAME} \
            -create -output ${OUTPUT}/dynamic/${NAME}.framework/${NAME} | echo
    fi
else
    if [[ ${BUILD_STATIC} == true ]]; then
        step "Assembling static library for iOS Simulator…"
        mkdir -p ${OUTPUT}/static/${NAME}.framework
        libtool -static -no_warning_for_no_symbols \
            `find mason_packages/ios-${IOS_SDK_VERSION} -type f -name libgeojsonvt.a` \
            -o ${OUTPUT}/static/${NAME}.framework/${NAME} \
            ${LIBS[@]/#/${PRODUCTS}/${BUILDTYPE}-iphonesimulator/libmbgl-}
    fi

    if [[ ${BUILD_DYNAMIC} == true ]]; then
        step "Copying dynamic framework into place for iOS Simulator…"
        cp -r \
            ${PRODUCTS}/${BUILDTYPE}-iphonesimulator/${NAME}.framework \
            ${OUTPUT}/dynamic/${NAME}.framework
        if [[ -e ${PRODUCTS}/${BUILDTYPE}-iphonesimulator/${NAME}.framework.dSYM ]]; then
            cp -r ${PRODUCTS}/${BUILDTYPE}-iphonesimulator/${NAME}.framework.dSYM \
                ${OUTPUT}/dynamic/
        fi
    fi
fi

if [[ "${GCC_GENERATE_DEBUGGING_SYMBOLS}" == false ]]; then
    step "Stripping binaries…"
    if [[ ${BUILD_STATIC} == true ]]; then
        strip -Sx "${OUTPUT}/static/${NAME}.framework/${NAME}"
    fi
    if [[ ${BUILD_DYNAMIC} == true ]]; then
        strip -Sx "${OUTPUT}/dynamic/${NAME}.framework/${NAME}"
    fi
fi

if [[ ${BUILD_STATIC} == true ]]; then
    stat "${OUTPUT}/static/${NAME}.framework"
fi
if [[ ${BUILD_DYNAMIC} == true ]]; then
    stat "${OUTPUT}/dynamic/${NAME}.framework"
fi

if [[ ${BUILD_STATIC} == true ]]; then
    step "Copying static library headers…"
    cp -rv "${PRODUCTS}/${BUILDTYPE}-iphoneos/Headers" "${OUTPUT}/static/${NAME}.framework/Headers"
    cat platform/ios/framework/Mapbox-static.h > "${OUTPUT}/static/${NAME}.framework/Headers/Mapbox.h"
    cat "${PRODUCTS}/${BUILDTYPE}-iphoneos/Headers/Mapbox.h" >> "${OUTPUT}/static/${NAME}.framework/Headers/Mapbox.h"
fi

step "Copying library resources…"
SEM_VERSION=$( git describe --tags --match=ios-v*.*.* --abbrev=0 | sed 's/^ios-v//' )
SHORT_VERSION=${SEM_VERSION%-*}
if [[ ${BUNDLE_RESOURCES} ]]; then
    cp -pv LICENSE.md ${STATIC_SETTINGS_DIRECTORY}
    cp -rv platform/ios/framework/Settings.bundle ${STATIC_SETTINGS_DIRECTORY}
else
    cp -pv LICENSE.md "${OUTPUT}"
    cp -rv platform/ios/framework/Settings.bundle "${OUTPUT}"
fi
if [[ ${BUILD_STATIC} == true ]]; then
    mkdir -p ${STATIC_BUNDLE_PATH}
    cp -pv platform/{default,ios}/resources/* ${STATIC_BUNDLE_PATH}
    INFO_PLIST_PATH="${OUTPUT}/static/${NAME}.framework/Info.plist"
    cp -pv platform/ios/framework/Info.plist "${INFO_PLIST_PATH}"
    plutil -remove CFBundleExecutable "${INFO_PLIST_PATH}"
    plutil -remove CFBundlePackageType "${INFO_PLIST_PATH}"
    plutil -replace CFBundleIdentifier -string com.mapbox.sdk.ios "${INFO_PLIST_PATH}"
    plutil -replace CFBundleName -string ${NAME} "${INFO_PLIST_PATH}"
    plutil -replace CFBundleShortVersionString -string "${SHORT_VERSION}" "${INFO_PLIST_PATH}"
    plutil -replace CFBundleVersion -string ${PROJ_VERSION} "${INFO_PLIST_PATH}"
    plutil -replace MGLSemanticVersionString -string "${SEM_VERSION}" "${INFO_PLIST_PATH}"
    plutil -replace MGLCommitHash -string "${HASH}" "${INFO_PLIST_PATH}"
    if [[ ${BUNDLE_RESOURCES} ]]; then
        cp -pv "${INFO_PLIST_PATH}" "${STATIC_BUNDLE_PATH}/Info.plist"
    fi
    mkdir "${OUTPUT}/static/${NAME}.framework/Modules"
    cp -pv platform/ios/framework/modulemap "${OUTPUT}/static/${NAME}.framework/Modules/module.modulemap"
fi
if [[ ${BUILD_DYNAMIC} == true ]]; then
    plutil -replace CFBundleShortVersionString -string "${SHORT_VERSION}" "${OUTPUT}/dynamic/${NAME}.framework/Info.plist"
    plutil -replace CFBundleVersion -string "${PROJ_VERSION}" "${OUTPUT}/dynamic/${NAME}.framework/Info.plist"
    plutil -replace MGLSemanticVersionString -string "${SEM_VERSION}" "${OUTPUT}/dynamic/${NAME}.framework/Info.plist"
    plutil -replace MGLCommitHash -string "${HASH}" "${OUTPUT}/dynamic/${NAME}.framework/Info.plist"
    cp -pv platform/ios/framework/strip-frameworks.sh "${OUTPUT}/dynamic/${NAME}.framework/strip-frameworks.sh"
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

step "Generating API documentation…"
make idocument OUTPUT="${OUTPUT}/documentation"
