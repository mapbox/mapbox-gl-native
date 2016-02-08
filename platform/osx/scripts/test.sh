#!/usr/bin/env bash

set -e
set -o pipefail
set -u

OSX_SDK_VERSION=`xcrun --sdk macosx --show-sdk-version`
OSX_PROJ_PATH=./build/osx-x86_64/gyp/osx.xcodeproj

export BUILDTYPE=${BUILDTYPE:-Release}

if [[ ! -e "${OSX_PROJ_PATH}/xcshareddata/xcschemes/osxsdk.xcscheme" ]]; then
    # Generate schemes
    open -g "${OSX_PROJ_PATH}"
    sleep 20
    
    # Share osxsdk scheme
    mkdir -pv "${OSX_PROJ_PATH}/xcshareddata/xcschemes/"
    mv -v \
        "${OSX_PROJ_PATH}/xcuserdata/${USER}.xcuserdatad/xcschemes/osxsdk.xcscheme" \
        "${OSX_PROJ_PATH}/xcshareddata/xcschemes/"
fi

xcodebuild -verbose \
    -sdk macosx${OSX_SDK_VERSION} \
    -project "${OSX_PROJ_PATH}" \
    -scheme osxsdk \
    test
