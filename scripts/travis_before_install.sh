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
    sudo add-apt-repository --yes ppa:boost-latest/ppa

    mapbox_time "apt_update" \
    sudo apt-get update -y

    mapbox_time "install_gcc" \
    sudo apt-get -y install gcc-4.8 g++-4.8

    mapbox_time "install_build_tools" \
    sudo apt-get -y install git build-essential zlib1g-dev automake gdb \
                            libtool xutils-dev make cmake pkg-config python-pip \
                            libboost1.55-dev libboost-regex1.55-dev libcurl4-openssl-dev \
                            libpng-dev libsqlite3-dev

    mapbox_time "install_mesa_deps" \
    sudo apt-get install -y libpthread-stubs0-dev \
                            xserver-xorg-dev x11proto-xinerama-dev libx11-xcb-dev \
                            libxcb-glx0-dev libxrender-dev llvm-3.4 mesa-utils && \
    sudo apt-get build-dep -y libgl1-mesa-dri libxcb-glx0-dev

    mapbox_time "build_mesa" \
    curl -sfo MesaLib-10.3.1.tar.gz http://ftp.de.debian.org/debian/pool/main/m/mesa/mesa_10.3.1.orig.tar.gz && \
    tar -zxf MesaLib-10.3.1.tar.gz && \
    cd Mesa-10.3.1 && \
    ./autogen.sh --with-gallium-drivers=svga,swrast --disable-dri \
        --enable-xlib-glx --enable-glx-tls --with-llvm-prefix=/usr/lib/llvm-3.4 \
        --without-va && \
    echo $CXXFLAGS
    echo $CFLAGS
    echo $LDFLAGS
    make -j2 && sudo make install && \
    cd ../

    mapbox_time "install_opengl" \
    sudo apt-get -y install libxi-dev x11proto-randr-dev \
                            x11proto-xext-dev libxrandr-dev \
                            x11proto-xf86vidmode-dev libxxf86vm-dev \
                            libxcursor-dev libxinerama-dev

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
