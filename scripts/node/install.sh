#!/usr/bin/env bash

set -e
set -o pipefail

source ./scripts/travis_helper.sh

if [[ ${OS} == 'linux' ]] ; then
    mapbox_time "update_apt" \
    sudo apt-get -y update

    mapbox_time "install_apt" \
    sudo apt-get -y install mesa-utils llvm-3.4 pkg-config libcurl4-openssl-dev \
        libboost-dev libboost-program-options-dev libxi-dev x11proto-randr-dev \
        x11proto-xext-dev libxrandr-dev x11proto-xf86vidmode-dev libxxf86vm-dev \
        libxcursor-dev libxinerama-dev
fi

mapbox_time "checkout_mason" \
git submodule update --init .mason
export PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"

if [[ ${OS} == 'linux' ]] ; then
    mapbox_time "install_mesa" \
    mason install mesa 10.4.3
elif [[ ${OS} == 'osx' ]] ; then
    if ! gem spec xcpretty > /dev/null 2>&1; then
        mapbox_time "install_xcpretty" \
        sudo gem install xcpretty --no-rdoc --no-ri --no-document --quiet
    fi
fi
