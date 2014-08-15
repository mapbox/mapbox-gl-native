#!/usr/bin/env bash

set -e
set -o pipefail

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    #
    # install Linux dependencies
    #
    if [[ `lsb_release -r` =~ "12.04" ]]; then
        sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
    fi
    sudo apt-get update -y
    sudo apt-get -y install gcc-4.8 g++-4.8
    sudo apt-get -y install git build-essential zlib1g-dev automake \
                            libtool xutils-dev make cmake pkg-config python-pip
    sudo apt-get -y install libxi-dev libglu1-mesa-dev x11proto-randr-dev \
                            x11proto-xext-dev libxrandr-dev \
                            x11proto-xf86vidmode-dev libxxf86vm-dev libxcursor-dev
    sudo pip install awscli

    #
    # setup flags
    #
    export DISPLAY=:99.0
    sh -e /etc/init.d/xvfb start

    #
    # turn on sanitizers during debug builds
    #
    if [[ ${BUILDTYPE} == "Debug" ]]; then
        if [[ ${CXX} == "g++" ]]; then
            export CXXFLAGS="-fsanitize=address ${CXXFLAGS}"
            export CFLAGS="-fsanitize=address ${CFLAGS}"
            export LDFLAGS="-fsanitize=address  ${LDFLAGS}"
        elif [[ ${CXX} == "clang++" ]]; then
            export CXXFLAGS="-fsanitize=thread -fPIC ${CXXFLAGS}"
            export CFLAGS="-fsanitize=thread ${CFLAGS}"
            export LDFLAGS="-fsanitize=thread -pie ${LDFLAGS}"
        fi
    fi
elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    #
    # install OS X dependencies
    #
    brew install autoconf automake libtool makedepend cmake pkg-config node git
fi

#
# turn off a few warnings
#
export CXXFLAGS="-Wno-unknown-warning-option -Wno-unused-local-typedefs -Wno-unknown-pragmas ${CXXFLAGS}";