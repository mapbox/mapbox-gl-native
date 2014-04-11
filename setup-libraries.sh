#!/bin/sh
set -e -u

if [ ! `which aclocal` ] || [ ! `which automake` ] || [ ! `which autoconf` ] || [ ! `which glibtool` ]; then
    echo 'autotools commands not found: run "brew install autoconf automake libtool" before continuing'
    exit 1
fi

if [ ! -d 'mapnik-packaging' ]; then
  git clone --depth=1 https://github.com/mapnik/mapnik-packaging.git
fi

cd mapnik-packaging/osx/
git pull

export CXX11=true

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
    if [ ! -d out/build-cpp11-libcpp-x86_64/include/boost ] ; then ./scripts/build_boost.sh `pwd`/../../src/ `pwd`/../../linux/ ; fi
    echo '     ...done'

./scripts/make_universal.sh

cd ../../
./configure \
--pkg-config-root=`pwd`/mapnik-packaging/osx/out/build-cpp11-libcpp-universal/lib/pkgconfig \
--boost=`pwd`/mapnik-packaging/osx/out/build-cpp11-libcpp-universal/