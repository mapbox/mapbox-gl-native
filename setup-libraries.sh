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
if [ ${UNAME} = 'Darwin' ]; then
    ensure_dep makedepend
    if [[ ! `which libtool` ]] && [[ ! `which glibtool` ]]; then
        MISSING_DEPS="$MISSING_DEPS libtool"
    fi
elif [ ${UNAME} = 'Linux' ]; then
    ensure_dep libtool
    ensure_dep gccmakedep
fi

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

if [ ! -d 'mapnik-packaging/.git' ]; then
  git clone --depth=1 https://github.com/mapnik/mapnik-packaging.git
fi

cd mapnik-packaging/osx/
git pull

export CXX11=true

if [ ${UNAME} = 'Darwin' ]; then
source iPhoneOS.sh
    if [ ! -f out/build-cpp11-libcpp-armv7/lib/libpng.a ] ; then ./scripts/build_png.sh ; fi
    if [ ! -f out/build-cpp11-libcpp-armv7/lib/libuv.a ] ; then ./scripts/build_libuv.sh ; fi
    echo '     ...done'

source iPhoneOSs.sh
    if [ ! -f out/build-cpp11-libcpp-armv7s/lib/libpng.a ] ; then ./scripts/build_png.sh ; fi
    if [ ! -f out/build-cpp11-libcpp-armv7s/lib/libuv.a ] ; then ./scripts/build_libuv.sh ; fi
    echo '     ...done'

source iPhoneOS64.sh
    if [ ! -f out/build-cpp11-libcpp-arm64/lib/libpng.a ] ; then ./scripts/build_png.sh ; fi
    if [ ! -f out/build-cpp11-libcpp-arm64/lib/libuv.a ] ; then ./scripts/build_libuv.sh ; fi
    echo '     ...done'

source iPhoneSimulator.sh
    if [ ! -f out/build-cpp11-libcpp-i386/lib/libpng.a ] ; then ./scripts/build_png.sh ; fi
    if [ ! -f out/build-cpp11-libcpp-i386/lib/libuv.a ] ; then ./scripts/build_libuv.sh ; fi
    echo '     ...done'

source MacOSX.sh
    if [ ! -f out/build-cpp11-libcpp-x86_64/lib/libpng.a ] ; then ./scripts/build_png.sh ; fi
    if [ ! -f out/build-cpp11-libcpp-x86_64/lib/libglfw3.a ] ; then ./scripts/build_glfw.sh ; fi
    if [ ! -f out/build-cpp11-libcpp-x86_64/lib/libuv.a ] ; then ./scripts/build_libuv.sh ; fi
    if [ ! -f out/build-cpp11-libcpp-x86_64/lib/libssl.a ] ; then ./scripts/build_openssl.sh ; fi
    if [ ! -f out/build-cpp11-libcpp-x86_64/lib/libcurl.a ] ; then ./scripts/build_curl.sh ; fi
    if [ ! -d out/build-cpp11-libcpp-x86_64/include/boost ] ; then ./scripts/build_boost.sh `pwd`/../../src/ `pwd`/../../include/ `pwd`/../../linux/ `pwd`/../../common/ ; fi
    echo '     ...done'

./scripts/make_universal.sh

cd ../../
./configure \
--pkg-config-root=`pwd`/mapnik-packaging/osx/out/build-cpp11-libcpp-universal/lib/pkgconfig \
--boost=`pwd`/mapnik-packaging/osx/out/build-cpp11-libcpp-universal

elif [ ${UNAME} = 'Linux' ]; then

source Linux.sh
    if [ ! -f out/build-cpp11-libstdcpp-gcc-x86_64/lib/libglfw3.a ] ; then ./scripts/build_glfw.sh ; fi
    if [ ! -f out/build-cpp11-libstdcpp-gcc-x86_64/lib/libpng.a ] ; then ./scripts/build_png.sh ; fi
    if [ ! -f out/build-cpp11-libstdcpp-gcc-x86_64/lib/libuv.a ] ; then ./scripts/build_libuv.sh ; fi
    if [ ! -f out/build-cpp11-libstdcpp-gcc-x86_64/lib/libssl.a ] ; then ./scripts/build_openssl.sh ; fi
    if [ ! -f out/build-cpp11-libstdcpp-gcc-x86_64/lib/libcurl.a ] ; then ./scripts/build_curl.sh ; fi
    if [ ! -d out/build-cpp11-libstdcpp-gcc-x86_64/include/boost ] ; then ./scripts/build_boost.sh `pwd`/../../src/ `pwd`/../../include/ `pwd`/../../linux/ `pwd`/../../common/ ; fi
    if [ ! -d out/build-cpp11-libstdcpp-gcc-x86_64/lib/libboost_regex.a ] ; then ./scripts/build_boost.sh --with-regex ; fi

cd ../../
./configure \
--pkg-config-root=`pwd`/mapnik-packaging/osx/out/build-cpp11-libstdcpp-gcc-x86_64/lib/pkgconfig \
--boost=`pwd`/mapnik-packaging/osx/out/build-cpp11-libstdcpp-gcc-x86_64

fi
