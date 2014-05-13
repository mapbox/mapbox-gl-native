#!/bin/bash
set -e -u


export CXX11=true

export SYSROOT=`pwd`/../rootfs
export SDK_PATH=`pwd`/../arm-linux-gnueabi-linaro_4.8.3-2014.04
export V=1

cd mapnik-packaging/osx/

source Linaro-softfp.sh
    echo $PATH
    if [ ! -f out/build-cpp11-libstdcpp-gcc-arm-linux-gnueabi/lib/libpng.a ] ; then ./scripts/build_png.sh ; fi
    # if [ ! -f out/build-cpp11-libstdcpp-gcc-arm-linux-gnueabi/lib/libglfw3.a ] ; then ./scripts/build_glfw.sh ; fi
    if [ ! -f out/build-cpp11-libstdcpp-gcc-arm-linux-gnueabi/lib/libuv.a ] ; then ./scripts/build_libuv.sh ; fi
    if [ ! -f out/build-cpp11-libstdcpp-gcc-arm-linux-gnueabi/lib/libssl.a ] ; then ./scripts/build_openssl.sh ; fi
    # if [ ! -f out/build-cpp11-libstdcpp-gcc-x86_64/lib/libcurl.a ] ; then ./scripts/build_curl.sh ; fi
    # if [ ! -d out/build-cpp11-libstdcpp-gcc-arm-linux-gnueabi/include/boost ] ; then ./scripts/build_boost.sh `pwd`/../../src/ `pwd`/../../linux/ `pwd`/../../common/ ; fi



# cd ../../
# ./configure \
# --pkg-config-root=`pwd`/mapnik-packaging/osx/out/build-cpp11-libstdcpp-gcc-x86_64/lib/pkgconfig \
# --boost=`pwd`/mapnik-packaging/osx/out/build-cpp11-libstdcpp-gcc-x86_64

# fi
