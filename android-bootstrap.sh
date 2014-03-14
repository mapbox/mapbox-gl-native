
if [[ ! -f android-ndk-r9c-darwin-x86_64.tar.bz2 ]]; then
    curl -O http://dl.google.com/android/ndk/android-ndk-r9c-darwin-x86_64.tar.bz2
fi

export NDK_PATH=$(pwd)/android-ndk-r9c

if [[ ! -d "${NDK_PATH}" ]]; then
    tar -xf android-ndk-r9c-darwin-x86_64.tar.bz2
fi

export ANDROID_PATH=$(pwd)/android-ndk-r9c
export NDK_PROJECT_PATH=$(pwd)
export APP_BUILD_SCRIPT=$(pwd)Android.mk

# see all API levels at $NDK_PATH/platforms/
export API_LEVEL="android-19"

# see all compiler options at $NDK_PATH/toolchains/
export ANDROID_CROSS_COMPILER="arm-linux-androideabi-clang3.3"
export PLATFORM_PREFIX="${NDK_PATH}/platforms/active-platform/"

if [[ ! -d "${PLATFORM_PREFIX}" ]]; then
    # NOTE: make-standalone-toolchain.sh --help for options
    ${NDK_PATH}/build/tools/make-standalone-toolchain.sh  \
      --toolchain=${ANDROID_CROSS_COMPILER} \
      --install-dir="${PLATFORM_PREFIX}" \
      --stl=gnustl \
      --arch=arm \
      --platform="${API_LEVEL}"
fi

ADT_BUNDLE="$(pwd)/adt-bundle-mac-x86_64-20131030"
PATH="${PLATFORM_PREFIX}/bin:${NDK_PATH}:${ADT_BUNDLE}/sdk/tools:${ADT_BUNDLE}/sdk/platform-tools":${PATH}
export PATH
echo $PATH

cd libpngtmp

if [[ ! -f libpng-1.6.9.tar.gz ]]; then
    curl -O http://mapnik.s3.amazonaws.com/deps/libpng-1.6.9.tar.gz
fi

rm -rf libpng-1.6.9
tar xzf libpng-1.6.9.tar.gz
cd libpng-1.6.9
./configure \
    --host=arm-linux-androideabi \
    CC=arm-linux-androideabi-clang \
    STRIP=arm-linux-androideabi-strip \
    --prefix=${PLATFORM_PREFIX} \
    AR=arm-linux-androideabi-ar \
    LD=arm-linux-androideabi-ld \
    RANLIB=arm-linux-androideabi-ranlib
make
make install
cd ../../
cp ${PLATFORM_PREFIX}/lib/libpng* jni/
mkdir -p jni/include
cp -r ${PLATFORM_PREFIX}/include/*png* jni/include/
