#!/bin/bash
set -e -u

UNAME=$(uname -s);

MISSING_DEPS=false

function ensure_dep {
    if [[ ! `which $1` ]]; then
        MISSING_DEPS="$MISSING_DEPS $1"
    fi
}

ensure_dep aclocal
ensure_dep cmake
ensure_dep automake
ensure_dep autoconf
if [ ${UNAME} = 'Darwin' ]; then
    ensure_dep makedepend
    ensure_dep glibtool
elif [ ${UNAME} = 'Linux' ]; then
    ensure_dep libtool
    ensure_dep gccmakedep
fi

if [[ $MISSING_DEPS != false ]]; then
    if [ ${UNAME} = 'Darwin' ]; then
        echo 'Please run "brew install autoconf automake libtool makedepend cmake" and then re-run ./setup-libraries.sh'
    elif [ ${UNAME} = 'Linux' ]; then
        echo 'Please run "sudo apt-get install automake libtool xutils-dev cmake" and then re-run ./setup-libraries.sh'
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
    if [ ! -d out/build-cpp11-libcpp-x86_64/include/boost ] ; then ./scripts/build_boost.sh `pwd`/../../src/ `pwd`/../../linux/ `pwd`/../../common/ ; fi
    echo '     ...done'

rm -rf out/packages

./scripts/make_universal.sh

cd ../../
./configure \
--pkg-config-root=`pwd`/mapnik-packaging/osx/out/build-cpp11-libcpp-universal/lib/pkgconfig \
--boost=`pwd`/mapnik-packaging/osx/out/build-cpp11-libcpp-universal

elif [ ${UNAME} = 'Linux' ]; then

source Linux.sh
    if [ ! -f out/build-cpp11-libstdcpp-gcc-x86_64/lib/libpng.a ] ; then ./scripts/build_png.sh ; fi
    if [ ! -f out/build-cpp11-libstdcpp-gcc-x86_64/lib/libglfw3.a ] ; then ./scripts/build_glfw.sh ; fi
    if [ ! -f out/build-cpp11-libstdcpp-gcc-x86_64/lib/libuv.a ] ; then ./scripts/build_libuv.sh ; fi
    if [ ! -f out/build-cpp11-libstdcpp-gcc-x86_64/lib/libssl.a ] ; then ./scripts/build_openssl.sh ; fi
    if [ ! -f out/build-cpp11-libstdcpp-gcc-x86_64/lib/libcurl.a ] ; then ./scripts/build_curl.sh ; fi
    if [ ! -d out/build-cpp11-libstdcpp-gcc-x86_64/include/boost ] ; then ./scripts/build_boost.sh `pwd`/../../src/ `pwd`/../../linux/ `pwd`/../../common/ ; fi

rm -rf out/packages

cd ../../
./configure \
--pkg-config-root=`pwd`/mapnik-packaging/osx/out/build-cpp11-libstdcpp-gcc-x86_64/lib/pkgconfig \
--boost=`pwd`/mapnik-packaging/osx/out/build-cpp11-libstdcpp-gcc-x86_64

fi
