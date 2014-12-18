#!/usr/bin/env bash

set -x
set -e
set -o pipefail

if [[ "${MASON_PLATFORM}" == "android" ]]; then

    if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then

        echo "load submodules"
        git submodule update --init --recursive

        echo "installing 7z"
        MASON_PLATFORM= ./.mason/mason install 7z 9.20

        echo "setting 7z path"
        SEVEN_ZIP_PATH="$(MASON_PLATFORM= ./.mason/mason prefix 7z 9.20)/bin/7za"

        echo "fetching NDK"
        wget http://dl.google.com/android/ndk/android-ndk-r10c-linux-x86_64.bin

        echo "chmod NDK"
        chmod a+x ./android-ndk-r10c-linux-x86_64.bin

        echo "unpacking NDK"
        $SEVEN_ZIP_PATH x ./android-ndk-r10c-linux-x86_64.bin > .tmp-ndk-log
        rm .tmp-ndk-log

        echo "fetching JDK"
        wget --no-cookies --no-check-certificate --header "Cookie: gpw_e24=http%3A%2F%2Fwww.oracle.com%2F; oraclelicense=accept-securebackup-cookie" http://download.oracle.com/otn-pub/java/jdk/7u71-b14/jdk-7u71-linux-x64.tar.gz

        echo "unpacking JDK"
        tar -xzf ./jdk-7u71-linux-x64.tar.gz

        echo "fetching SDK"
        wget http://dl.google.com/android/android-sdk_r23.0.2-linux.tgz

        echo "unpacking SDK"
        tar -xzf ./android-sdk_r23.0.2-linux.tgz
        mv ./android-sdk-linux ./android-sdk

        echo "installing SDK"
        sudo apt-get update -y
        sudo apt-get -y install lib32stdc++6 lib32z1
        echo y | ./android-sdk/tools/android update sdk -u -a -t tools,platform-tools,build-tools-21.1.2,android-21,extra-android-m2repository,extra-google-m2repository

        echo "inserting access token"
        echo "${ANDROID_KEY}" >> ./android/java/app/src/main/res/token.txt

    elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then

        echo "installing 7z"
        MASON_PLATFORM= ./.mason/mason install 7z 9.20

        echo "setting 7z path"
        SEVEN_ZIP_PATH="$(MASON_PLATFORM= ./.mason/mason prefix 7z 9.20)/bin/7za"

        echo "fetching NDK"
        wget http://dl.google.com/android/ndk/android-ndk-r10c-darwin-x86_64.bin

        echo "chmod NDK"
        chmod a+x ./android-ndk-r10c-darwin-x86_64.bin

        sudo ps aux

        echo "unpacking NDK"
        $SEVEN_ZIP_PATH x ./android-ndk-r10c-darwin-x86_64.bin > .tmp-ndk-log
        rm .tmp-ndk-log

        echo "fetching SDK"
        wget http://dl.google.com/android/android-sdk_r23.0.2-macosx.zip

        echo "unpacking SDK"
        unzip -qq android-sdk_r23.0.2-macosx.zip
        mv ./android-sdk-macosx ./android-sdk

        echo "installing SDK"
        echo y | ./android-sdk/tools/android update sdk -u -a -t tools,platform-tools,build-tools-21.1.2,android-21,extra-android-m2repository,extra-google-m2repository

        echo "inserting access token"
        echo "${ANDROID_KEY}" >> ./android/java/app/src/main/res/token.txt

    fi

fi

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    #
    # install Linux dependencies
    #
    if [[ `lsb_release -r` =~ "12.04" ]]; then
        sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
    fi
    sudo add-apt-repository --yes ppa:boost-latest/ppa

    echo "apt_update"
    sudo apt-get update -y

    echo "install_gcc"
    sudo apt-get -y install gcc-4.8 g++-4.8

    echo "install_build_tools"
    sudo apt-get -y install git build-essential zlib1g-dev automake gdb \
                            libtool xutils-dev make cmake pkg-config python-pip \
                            libboost1.55-dev libcurl4-openssl-dev \
                            libpng-dev libsqlite3-dev

    if [[ ${MASON_PLATFORM} != "android" ]]; then
        echo "install_opengl"
        sudo apt-get -y install mesa-utils libxi-dev x11proto-randr-dev \
                                x11proto-xext-dev libxrandr-dev \
                                x11proto-xf86vidmode-dev libxxf86vm-dev \
                                libxcursor-dev libxinerama-dev \
                                llvm-3.4 # required for mesa

        echo "install_mesa"
        MASON_PLATFORM= mason install mesa 10.3.1
    fi

    echo "install_awscli"
    sudo pip install awscli
elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    #
    # install OS X dependencies
    #
    mapbox_time "install_build_tools" \
    brew install makedepend git

    mapbox_time "install_awscli" \
    sudo pip install awscli
fi
