#!/bin/bash
set -e -u

UNAME=$(uname -s);

MISSING_DEPS=""

function ensure_dep {
    if [[ ! `which $1` ]]; then
        MISSING_DEPS="$MISSING_DEPS $1"
    fi
}

ensure_dep aclocal
ensure_dep cmake
ensure_dep automake
ensure_dep autoconf
ensure_dep pkg-config
ensure_dep node
ensure_dep libtool
ensure_dep gccmakedep

if [[ $MISSING_DEPS != "" ]]; then
    if [ ${UNAME} = 'Darwin' ]; then
        echo "Missing build deps: ${MISSING_DEPS}"
        echo 'Please run "brew install autoconf automake libtool makedepend cmake pkg-config node"'
        echo 'and then re-run ./setup-libraries.sh'
    elif [ ${UNAME} = 'Linux' ]; then
        echo "Missing build deps: ${MISSING_DEPS}"
        echo 'Please run "sudo apt-get install git build-essential zlib1g-dev automake libtool xutils-dev make cmake pkg-config nodejs-legacy libxi-dev libglu1-mesa-dev x11proto-randr-dev x11proto-xext-dev libxrandr-dev x11proto-xf86vidmode-dev libxxf86vm-dev libxcursor-dev"'
        echo 'and then re-run ./setup-libraries.sh'
    fi
    exit 1
fi

export SYSROOT=`pwd`/../rootfs
export SDK_PATH=`pwd`/../arm-linux-gnueabi-linaro_4.8.3-2014.04
export V=1

if [ ! -d 'mapnik-packaging/.git' ]; then
  git clone --depth=1 https://github.com/mapnik/mapnik-packaging.git
fi

cd mapnik-packaging/osx/
# git pull

export CXX11=true

source Linaro-softfp.sh
    if [ ! -f out/build-cpp11-libstdcpp-gcc-arm-linux-gnueabi-linaro-softfp/lib/libglfw3.a ] ; then ./scripts/build_glfw.sh ; fi
    if [ ! -f out/build-cpp11-libstdcpp-gcc-arm-linux-gnueabi-linaro-softfp/lib/libpng.a ] ; then ./scripts/build_png.sh ; fi
    if [ ! -f out/build-cpp11-libstdcpp-gcc-arm-linux-gnueabi-linaro-softfp/lib/libuv.a ] ; then ./scripts/build_libuv.sh ; fi
    if [ ! -f out/build-cpp11-libstdcpp-gcc-arm-linux-gnueabi-linaro-softfp/lib/libssl.a ] ; then ./scripts/build_openssl.sh ; fi
    if [ ! -f out/build-cpp11-libstdcpp-gcc-arm-linux-gnueabi-linaro-softfp/lib/libcurl.a ] ; then ./scripts/build_curl.sh ; fi
    if [ ! -f out/build-cpp11-libstdcpp-gcc-arm-linux-gnueabi-linaro-softfp/lib/libboost_regex.a ] ; then ./scripts/build_boost.sh --with-regex ; fi

cd ../../
./configure \
--pkg-config-root=`pwd`/mapnik-packaging/osx/out/build-cpp11-libstdcpp-gcc-arm-linux-gnueabi-linaro-softfp/lib/pkgconfig \
--boost=`pwd`/mapnik-packaging/osx/out/build-cpp11-libstdcpp-gcc-arm-linux-gnueabi-linaro-softfp \
--node=`which node`

