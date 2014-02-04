#!/bin/sh

cd ../
if [ ! -d 'mapnik-packaging' ]; then
  git clone --depth=0 https://github.com/mapnik/mapnik-packaging.git
fi
cd mapnik-packaging/osx/
export CXX11=true
(source iPhoneSimulator.sh; ./scripts/build_png.sh; \
 source MacOSX.sh; 			./scripts/build_png.sh; \
 source iPhoneOS.sh; 		./scripts/build_png.sh; \
 source iPhoneOSs.sh;  		./scripts/build_png.sh; \
 source iPhoneOS64.sh;  	./scripts/build_png.sh; \
 ./scripts/make_universal.sh)
export UNIVERSAL_LIBS=`pwd`/out/build-cpp11-libcpp-universal/
export PNG_INCLUDES=`pwd`/out/build-cpp11-libcpp-i386/
cd ../../
./configure --png-libpath=${UNIVERSAL_LIBS} --png-includes=${PNG_INCLUDES}