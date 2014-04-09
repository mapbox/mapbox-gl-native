#!/bin/sh
set -e -u

if [ ! -d 'mapnik-packaging' ]; then
  git clone --depth=0 https://github.com/mapnik/mapnik-packaging.git
fi

cd mapnik-packaging/osx/
git pull

export CXX11=true

source iPhoneOS.sh
    ./scripts/build_png.sh
    ./scripts/build_libuv.sh

source iPhoneOSs.sh
    ./scripts/build_png.sh
    ./scripts/build_libuv.sh

source iPhoneOS64.sh
    ./scripts/build_png.sh
    ./scripts/build_libuv.sh

source iPhoneSimulator.sh
    ./scripts/build_png.sh
    ./scripts/build_libuv.sh

source MacOSX.sh
    ./scripts/build_png.sh
    ./scripts/build_libuv.sh
    ./scripts/build_glfw.sh
    ./scripts/build_openssl.sh
    ./scripts/build_curl.sh

./scripts/make_universal.sh

cd ../../
./configure --pkg-config-root=`pwd`/mapnik-packaging/osx/out/build-cpp11-libcpp-universal/lib/pkgconfig
