
set -u

export NDK_VERSION=android-ndk-r9c
export NDK_PATH=$(pwd)/${NDK_VERSION}
export ANDROID_PATH=${NDK_PATH}
export API_LEVEL="android-19"
export ANDROID_CROSS_COMPILER="arm-linux-androideabi-4.8"
export PLATFORM_PREFIX="${NDK_PATH}/platforms/active-platform/"
export PATH="${PLATFORM_PREFIX}/bin:${NDK_PATH}":${PATH}

if [[ ! -f ${NDK_VERSION}-darwin-x86_64.tar.bz2 ]]; then
    echo "downloading ${NDK_VERSION}"
    curl -O http://dl.google.com/android/ndk/${NDK_VERSION}-darwin-x86_64.tar.bz2
fi


if [[ ! -d "${NDK_PATH}" ]]; then
    echo "untarring ${NDK_VERSION}"
    tar -xf ${NDK_VERSION}-darwin-x86_64.tar.bz2
fi

export NDK_PROJECT_PATH=$(pwd)
export APP_BUILD_SCRIPT=$(pwd)Android.mk

if [[ ! -d "${PLATFORM_PREFIX}" ]]; then
    echo "configure platform for ${NDK_VERSION}"
    # NOTE: make-standalone-toolchain.sh --help for options
    ${NDK_PATH}/build/tools/make-standalone-toolchain.sh  \
      --toolchain=${ANDROID_CROSS_COMPILER} \
      --install-dir="${PLATFORM_PREFIX}" \
      --stl=gnustl \
      --arch=arm \
      --platform="${API_LEVEL}"
fi

ADT_BUNDLE="$(pwd)/adt-bundle-mac-x86_64-20131030"
export PATH="${ADT_BUNDLE}/sdk/tools:${ADT_BUNDLE}/sdk/platform-tools":${PATH}

cd libpngtmp

if [[ ! -f libpng-1.6.9.tar.gz ]]; then
    curl -O http://mapnik.s3.amazonaws.com/deps/libpng-1.6.9.tar.gz
fi

echo "compiling libpng for ${NDK_VERSION}"
rm -rf libpng-1.6.9
tar xzf libpng-1.6.9.tar.gz
cd libpng-1.6.9
./configure \
    --host=arm-linux-androideabi \
    CC=arm-linux-androideabi-gcc \
    STRIP=arm-linux-androideabi-strip \
    --prefix=${PLATFORM_PREFIX} \
    AR=arm-linux-androideabi-ar \
    LD=arm-linux-androideabi-ld \
    RANLIB=arm-linux-androideabi-ranlib  1> /dev/null
make 1> /dev/null
make install 1> /dev/null
cd ../../

echo "busy doing some dodgy business"
cp ${PLATFORM_PREFIX}/lib/libpng* jni/
mkdir -p jni/include
cp -r ${PLATFORM_PREFIX}/include/*png* jni/include/
cp -r src jni/
echo "finally running ndk-build"
ndk-build
