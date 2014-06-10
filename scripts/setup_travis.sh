#!/usr/bin/env bash

set -e
set -o pipefail

# enable testing locally (not on travis)
if [[ "${TRAVIS_OS_NAME:-false}" == false ]]; then
    if [[ $(uname -s) == "Darwin" ]]; then
        TRAVIS_OS_NAME="osx"
    elif [[ $(uname -s) == "Linux" ]]; then
        TRAVIS_OS_NAME="linux"
    fi
fi

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then

    if [[ `lsb_release -r` =~ "12.04" ]]; then
        sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
    fi
    sudo apt-get update -y
    sudo apt-get -y install gcc-4.8 g++-4.8
    sudo apt-get -y install git build-essential zlib1g-dev automake libtool xutils-dev make cmake pkg-config
    sudo apt-get -y install libxi-dev libglu1-mesa-dev x11proto-randr-dev x11proto-xext-dev libxrandr-dev x11proto-xf86vidmode-dev libxxf86vm-dev libxcursor-dev

    # use g++ that supports c++11
    if [[ ${CXX} == "g++" ]]; then
        export CXX="g++-4.8" ;
    fi

    # If building in debug then turn on sanitizers.
    # They should both work with latest g++/clang++
    # but it's not worth a standalone matrix so here
    # we just test address->gcc and thread->clang for
    # some limited coverage
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
    brew install autoconf automake libtool makedepend cmake pkg-config node git
fi

# turn off a few warnings
export CXXFLAGS="-Wno-unknown-warning-option -Wno-unused-local-typedefs -Wno-unknown-pragmas ${CXXFLAGS}";
