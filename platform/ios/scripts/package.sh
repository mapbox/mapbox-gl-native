#!/usr/bin/env bash

set -e
set -o pipefail
set -u

NAME=Mapbox
OUTPUT=build/ios/pkg
DERIVED_DATA=build/ios
PRODUCTS=${DERIVED_DATA}

BUILDTYPE=${BUILDTYPE:-Debug}
BUILD_FOR_DEVICE=${BUILD_DEVICE:-true}
SYMBOLS=${SYMBOLS:-YES}

BUILD_DYNAMIC=true
BUILD_STATIC=true
if [[ ${FORMAT} == "static" ]]; then
    BUILD_DYNAMIC=false
elif [[ ${FORMAT} == "dynamic" ]]; then
    BUILD_STATIC=false
fi

SDK=iphonesimulator
if [[ ${BUILD_FOR_DEVICE} == true ]]; then
    SDK=iphoneos
fi
IOS_SDK_VERSION=`xcrun --sdk ${SDK} --show-sdk-version`

function step { >&2 echo -e "\033[1m\033[36m* $@\033[0m"; }
function finish { >&2 echo -en "\033[0m"; }
trap finish EXIT

step "Configuring ${FORMAT:-dynamic and static} ${BUILDTYPE} framework for ${SDK} ${IOS_SDK_VERSION}; symbols: ${SYMBOLS}"

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
if [[ ${BUILD_DYNAMIC} == true && ${BUILD_STATIC} == true ]]; then
    SCHEME+='+static'
elif [[ ${BUILD_STATIC} == true ]]; then
    SCHEME='static'
fi

step "Building for iOS Simulator using scheme ${SCHEME}"
xcodebuild \
    CURRENT_PROJECT_VERSION=${PROJ_VERSION} \
    CURRENT_SHORT_VERSION=${SHORT_VERSION} \
    CURRENT_SEMANTIC_VERSION=${SEM_VERSION} \
    CURRENT_COMMIT_HASH=${HASH} \
    ONLY_ACTIVE_ARCH=NO \
    -derivedDataPath ${DERIVED_DATA} \
    -workspace ./platform/ios/ios.xcworkspace \
    -scheme ${SCHEME} \
    -configuration ${BUILDTYPE} \
    -sdk iphonesimulator \
    -jobs ${JOBS} | xcpretty

if [[ ${BUILD_FOR_DEVICE} == true ]]; then
    step "Building for iOS devices using scheme ${SCHEME}"
    xcodebuild \
        CURRENT_PROJECT_VERSION=${PROJ_VERSION} \
        CURRENT_SHORT_VERSION=${SHORT_VERSION} \
        CURRENT_SEMANTIC_VERSION=${SEM_VERSION} \
        CURRENT_COMMIT_HASH=${HASH} \
        ONLY_ACTIVE_ARCH=NO \
        -derivedDataPath ${DERIVED_DATA} \
        -workspace ./platform/ios/ios.xcworkspace \
        -scheme ${SCHEME} \
        -configuration ${BUILDTYPE} \
        -sdk iphoneos \
        -jobs ${JOBS} | xcpretty
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

    cp -rv ${PRODUCTS}/${BUILDTYPE}-iphoneos/Settings.bundle ${OUTPUT}
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

    cp -rv ${PRODUCTS}/${BUILDTYPE}-iphonesimulator/Settings.bundle ${OUTPUT}
fi

if [[ ${SYMBOLS} = NO ]]; then
    step "Stripping symbols from binaries"
    if [[ ${BUILD_STATIC} == true ]]; then
        strip -Sx "${OUTPUT}/static/${NAME}.framework/${NAME}"
    fi
    if [[ ${BUILD_DYNAMIC} == true ]]; then
        strip -Sx "${OUTPUT}/dynamic/${NAME}.framework/${NAME}"
    fi
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

        # To-do: remove this in 4.0.0, as we intend to remove support for i386 entirely.
        step "Removing i386 slice from dSYM"
        lipo -remove i386 "${OUTPUT}/dynamic/${NAME}.framework.dSYM/Contents/Resources/DWARF/${NAME}" -o "${OUTPUT}/dynamic/${NAME}.framework.dSYM/Contents/Resources/DWARF/${NAME}"
        lipo -info "${OUTPUT}/dynamic/${NAME}.framework.dSYM/Contents/Resources/DWARF/${NAME}"
fi

function create_podspec {
    step "Creating local podspec (${1})"
    [[ $SYMBOLS = YES ]] && POD_SUFFIX="-symbols" || POD_SUFFIX=""
    POD_SOURCE_PATH='    :path => ".",'
    POD_FRAMEWORKS="  m.vendored_frameworks = '"${NAME}".framework'"
    INPUT_PODSPEC=platform/ios/${NAME}-iOS-SDK${POD_SUFFIX}.podspec
    OUTPUT_PODSPEC=${OUTPUT}/${1}/${NAME}-iOS-SDK${POD_SUFFIX}.podspec
    if [[ ${1} == "dynamic" ]]; then
        sed "s/.*:http.*/${POD_SOURCE_PATH}/" ${INPUT_PODSPEC} > ${OUTPUT_PODSPEC}
        sed -i '' "s/.*vendored_frameworks.*/${POD_FRAMEWORKS}/" ${OUTPUT_PODSPEC}
    fi
    if [[ ${1} == "static" ]]; then
        awk '/Pod::Spec.new/,/m.platform/' ${INPUT_PODSPEC} > ${OUTPUT_PODSPEC}
        cat platform/ios/${NAME}-iOS-SDK-static-part.podspec >> ${OUTPUT_PODSPEC}
        sed -i '' "s/.*:http.*/${POD_SOURCE_PATH}/" ${OUTPUT_PODSPEC}
    fi
    cp -pv LICENSE.md ${OUTPUT}/${1}/
}

if [[ ${BUILD_STATIC} == true ]]; then
    stat "${OUTPUT}/static/${NAME}.framework"
    create_podspec "static"
fi
if [[ ${BUILD_DYNAMIC} == true ]]; then
    stat "${OUTPUT}/dynamic/${NAME}.framework"
    create_podspec "dynamic"
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

    step "Copying demo project and sym linking to published framework…"
    cp -rv platform/ios/scripts/script_resources/MapboxDemo "${OUTPUT}"
    cd "${OUTPUT}/MapboxDemo"
    ln -sv "../dynamic/${NAME}.framework"
    cd -

    step "Building demo project…"
    xcodebuild -quiet -project build/ios/pkg/MapboxDemo/MapboxDemo.xcodeproj -scheme MapboxDemo build ONLY_ACTIVE_ARCH=YES -destination 'platform=iOS Simulator,name=iPhone 7' clean build &> /tmp/iosdemobuildoutput || true
    if grep -Fxq "** BUILD FAILED **" /tmp/iosdemobuildoutput
    then
        echo "Could not build demo project with this version of the SDK."
        rm -rf "${OUTPUT}/MapboxDemo"
    else
        echo "Built and packaged demo project."
    fi
    rm /tmp/iosdemobuildoutput
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
