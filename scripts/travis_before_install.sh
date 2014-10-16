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

    mapbox_time "apt_update" \
    sudo apt-get update -y

    mapbox_time "install_gcc" \
    sudo apt-get -y install gcc-4.8 g++-4.8

    mapbox_time "install_build_tools" \
    sudo apt-get -y install git build-essential zlib1g-dev automake \
                            libtool xutils-dev make cmake pkg-config python-pip

    mapbox_time "install_opengl" \
    sudo apt-get -y install libxi-dev libglu1-mesa-dev x11proto-randr-dev \
                            x11proto-xext-dev libxrandr-dev \
                            x11proto-xf86vidmode-dev libxxf86vm-dev libxcursor-dev

    mapbox_time "install_awscli" \
    sudo pip install awscli
elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    #
    # install OS X dependencies
    #
    mapbox_time "install_build_tools" \
    brew install autoconf automake libtool makedepend cmake pkg-config node git

    mapbox_time "install_awscli" \
    sudo pip install awscli
fi
