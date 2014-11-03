#!/usr/bin/env bash

set -e
set -o pipefail

if [[ "${MASON_PLATFORM}" == "android" ]]; then
    echo "debug: unpacking"
    #mapbox_time "unpacking NDK" \
    MASON_PLATFORM= ./.mason/mason install 7z 9.20

    echo "debug: setting 7z path"
    SEVEN_ZIP_PATH="$(MASON_PLATFORM= ./.mason/mason prefix 7z 9.20)/bin/7za"

    echo "debug: fetching NDK"
    #mapbox_time "fetching NDK" \
    wget http://dl.google.com/android/ndk/android-ndk-r10c-linux-x86_64.bin

    echo "debug: chmod NDK"
    chmod a+x ./android-ndk-r10c-linux-x86_64.bin

    echo "debug: upacking NDK"
    #mapbox_time "unpacking NDK" \
    #$SEVEN_ZIP_PATH x ./android-ndk-r10c-linux-x86_64.bin > /dev/null

    echo "debug: setting ANDROID_NDK_PATH"
    #export ANDROID_NDK_PATH=$(pwd)/android-ndk-r10c

    echo "debug: $ANDROID_NDK_PATH"
fi

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    #
    # install Linux dependencies
    #
    if [[ `lsb_release -r` =~ "12.04" ]]; then
        sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
    fi
    sudo add-apt-repository --yes ppa:boost-latest/ppa

    mapbox_time "apt_update" \
    sudo apt-get update -y

    mapbox_time "install_gcc" \
    sudo apt-get -y install gcc-4.8 g++-4.8

    mapbox_time "install_build_tools" \
    sudo apt-get -y install git build-essential zlib1g-dev automake gdb \
                            libtool xutils-dev make cmake pkg-config python-pip \
                            libboost1.55-dev libcurl4-openssl-dev \
                            libpng-dev libsqlite3-dev

    mapbox_time "install_opengl" \
    sudo apt-get -y install mesa-utils libxi-dev x11proto-randr-dev \
                            x11proto-xext-dev libxrandr-dev \
                            x11proto-xf86vidmode-dev libxxf86vm-dev \
                            libxcursor-dev libxinerama-dev \
                            llvm-3.4 # required for mesa


    # TODO - figure out how to unset MASON_PLATFORM for just this command
    #mapbox_time "install_mesa" \
    MASON_PLATFORM= mason install mesa 10.3.1

    mapbox_time "install_awscli" \
    sudo pip install awscli
elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    #
    # install OS X dependencies
    #
    mapbox_time "install_build_tools" \
    brew install autoconf automake libtool makedepend cmake pkg-config node git boost

    mapbox_time "install_awscli" \
    sudo pip install awscli
fi
