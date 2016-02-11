#!/usr/bin/env bash

set -e
set -o pipefail
set -u

NAME=Mapbox
OUTPUT=build/ios/pkg
LIBUV_VERSION=1.7.5

BUILDTYPE=${BUILDTYPE:-Release}
BUILD_FOR_DEVICE=${BUILD_DEVICE:-true}
ENABLE_BITCODE=${BITCODE:-YES}
GCC_GENERATE_DEBUGGING_SYMBOLS=${SYMBOLS:-YES}

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

echo "Configuring ${FORMAT:-dynamic and static} ${BUILDTYPE} framework for ${SDK}; symbols: ${GCC_GENERATE_DEBUGGING_SYMBOLS}; Bitcode: ${ENABLE_BITCODE}"

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


step "Creating build files…"
export MASON_PLATFORM=ios
export BUILDTYPE=${BUILDTYPE:-Release}
export HOST=ios
make Xcode/ios

PROJ_VERSION=$(git rev-list --count HEAD)

if [[ "${BUILD_FOR_DEVICE}" == true ]]; then
    if [[ ${BUILD_STATIC} == true ]]; then
        step "Building intermediate static libraries for iOS devices (build ${PROJ_VERSION})…"
        xcodebuild -sdk iphoneos${IOS_SDK_VERSION} \
            ARCHS="arm64 armv7 armv7s" \
            ONLY_ACTIVE_ARCH=NO \
            GCC_GENERATE_DEBUGGING_SYMBOLS=${GCC_GENERATE_DEBUGGING_SYMBOLS} \
            ENABLE_BITCODE=${ENABLE_BITCODE} \
            DEPLOYMENT_POSTPROCESSING=YES \
            CODE_SIGNING_REQUIRED=NO \
            CODE_SIGN_IDENTITY= \
            -project ./build/ios-all/gyp/mbgl.xcodeproj \
            -configuration ${BUILDTYPE} \
            -target everything \
            -jobs ${JOBS}
    fi
    
    if [[ ${BUILD_DYNAMIC} == true ]]; then
        step "Building dynamic framework for iOS devices (build ${PROJ_VERSION})…"
        xcodebuild -sdk iphoneos${IOS_SDK_VERSION} \
            ARCHS="arm64 armv7 armv7s" \
            ONLY_ACTIVE_ARCH=NO \
            GCC_GENERATE_DEBUGGING_SYMBOLS=${GCC_GENERATE_DEBUGGING_SYMBOLS} \
            ENABLE_BITCODE=${ENABLE_BITCODE} \
            DEPLOYMENT_POSTPROCESSING=YES \
            CURRENT_PROJECT_VERSION=${PROJ_VERSION} \
            CODE_SIGNING_REQUIRED=NO \
            CODE_SIGN_IDENTITY= \
            -project ./build/ios-all/gyp/ios.xcodeproj \
            -configuration ${BUILDTYPE} \
            -target iossdk \
            -jobs ${JOBS}
    fi
fi

if [[ ${BUILD_STATIC} == true ]]; then
    step "Building intermediate static libraries for iOS Simulator (build ${PROJ_VERSION})…"
    xcodebuild -sdk iphonesimulator${IOS_SDK_VERSION} \
        ARCHS="x86_64 i386" \
        ONLY_ACTIVE_ARCH=NO \
        GCC_GENERATE_DEBUGGING_SYMBOLS=${GCC_GENERATE_DEBUGGING_SYMBOLS} \
        -project ./build/ios-all/gyp/mbgl.xcodeproj \
        -configuration ${BUILDTYPE} \
        -target everything \
        -jobs ${JOBS}
fi

if [[ ${BUILD_DYNAMIC} == true ]]; then
    step "Building dynamic framework for iOS Simulator (build ${PROJ_VERSION})…"
    xcodebuild -sdk iphonesimulator${IOS_SDK_VERSION} \
        ARCHS="x86_64 i386" \
        ONLY_ACTIVE_ARCH=NO \
        GCC_GENERATE_DEBUGGING_SYMBOLS=${GCC_GENERATE_DEBUGGING_SYMBOLS} \
        ENABLE_BITCODE=${ENABLE_BITCODE} \
        CURRENT_PROJECT_VERSION=${PROJ_VERSION} \
        -project ./build/ios-all/gyp/ios.xcodeproj \
        -configuration ${BUILDTYPE} \
        -target iossdk \
        -jobs ${JOBS}
fi

LIBS=(core.a platform-ios.a asset-fs.a http-nsurl.a)

# https://medium.com/@syshen/create-an-ios-universal-framework-148eb130a46c
if [[ "${BUILD_FOR_DEVICE}" == true ]]; then
    if [[ ${BUILD_STATIC} == true ]]; then
        step "Assembling static framework for iOS Simulator and devices…"
        mkdir -p ${OUTPUT}/static/${NAME}.framework
        libtool -static -no_warning_for_no_symbols \
            `find mason_packages/ios-${IOS_SDK_VERSION} -type f -name libuv.a` \
            `find mason_packages/ios-${IOS_SDK_VERSION} -type f -name libgeojsonvt.a` \
            -o ${OUTPUT}/static/${NAME}.framework/${NAME} \
            ${LIBS[@]/#/gyp/build/${BUILDTYPE}-iphoneos/libmbgl-} \
            ${LIBS[@]/#/gyp/build/${BUILDTYPE}-iphonesimulator/libmbgl-}
    fi
    
    if [[ ${BUILD_DYNAMIC} == true ]]; then
        step "Copying dynamic framework into place for iOS devices"
        cp -r \
            gyp/build/${BUILDTYPE}-iphoneos/${NAME}.framework \
            ${OUTPUT}/dynamic/
        
        step "Merging simulator dynamic library into device dynamic library…"
        lipo \
            gyp/build/${BUILDTYPE}-iphoneos/${NAME}.framework/${NAME} \
            gyp/build/${BUILDTYPE}-iphonesimulator/${NAME}.framework/${NAME} \
            -create -output ${OUTPUT}/dynamic/${NAME}.framework/${NAME} | echo
    fi
else
    if [[ ${BUILD_STATIC} == true ]]; then
        step "Assembling static library for iOS Simulator…"
        mkdir -p ${OUTPUT}/static/${NAME}.framework
        libtool -static -no_warning_for_no_symbols \
            `find mason_packages/ios-${IOS_SDK_VERSION} -type f -name libuv.a` \
            `find mason_packages/ios-${IOS_SDK_VERSION} -type f -name libgeojsonvt.a` \
            -o ${OUTPUT}/static/${NAME}.framework/${NAME} \
            ${LIBS[@]/#/gyp/build/${BUILDTYPE}-iphonesimulator/libmbgl-}
    fi
    
    if [[ ${BUILD_DYNAMIC} == true ]]; then
        step "Copying dynamic framework into place for iOS Simulator…"
        cp -r \
            gyp/build/${BUILDTYPE}-iphonesimulator/${NAME}.framework \
            ${OUTPUT}/dynamic/${NAME}.framework
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
    mkdir -p "${OUTPUT}/static/${NAME}.framework/Headers"
    cp -pv platform/{darwin,ios}/include/*.h "${OUTPUT}/static/${NAME}.framework/Headers"
fi

step "Copying library resources…"
SHORT_VERSION=$( git describe --tags --match=ios-v*.*.* --abbrev=0 | sed 's/^ios-v//' )
cp -pv LICENSE.md "${OUTPUT}"
cp -rv platform/ios/app/Settings.bundle "${OUTPUT}"
if [[ ${BUILD_STATIC} == true ]]; then
    cp -pv platform/ios/resources/* "${OUTPUT}/static/${NAME}.framework"
    cp -pv platform/ios/framework/Info.plist "${OUTPUT}/static/${NAME}.framework/Info.plist"
    plutil -replace CFBundleExecutable -string ${NAME} "${OUTPUT}/static/${NAME}.framework/Info.plist"
    plutil -replace CFBundleIdentifier -string com.mapbox.sdk.ios "${OUTPUT}/static/${NAME}.framework/Info.plist"
    plutil -replace CFBundleName -string ${NAME} "${OUTPUT}/static/${NAME}.framework/Info.plist"
    plutil -replace CFBundleShortVersionString -string "${SHORT_VERSION}" "${OUTPUT}/static/${NAME}.framework/Info.plist"
    plutil -replace CFBundleVersion -string ${PROJ_VERSION} "${OUTPUT}/static/${NAME}.framework/Info.plist"
    mkdir "${OUTPUT}/static/${NAME}.framework/Modules"
    cp -pv platform/ios/framework/modulemap "${OUTPUT}/static/${NAME}.framework/Modules/module.modulemap"
fi
if [[ ${BUILD_DYNAMIC} == true ]]; then
    plutil -replace CFBundleShortVersionString -string "${SHORT_VERSION}" "${OUTPUT}/dynamic/${NAME}.framework/Info.plist"
    cp -pv platform/ios/framework/strip-frameworks.sh "${OUTPUT}/dynamic/${NAME}.framework/strip-frameworks.sh"
fi
sed -n -e '/^## iOS/,$p' CHANGELOG.md > "${OUTPUT}/CHANGELOG.md"

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
