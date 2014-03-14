curl -O http://dl.google.com/android/ndk/android-ndk-r9c-darwin-x86_64.tar.bz2
tar -xf android-ndk-r9c-darwin-x86_64.tar.bz2

export NDK_PATH=$(pwd)/android-ndk-r9c
export ANDROID_PATH=$(pwd)/android-ndk-r9c
export NDK_PROJECT_PATH=$(pwd)
export APP_BUILD_SCRIPT=$(pwd)Android.mk

# see all API levels at $NDK_PATH/platforms/
export API_LEVEL="android-19"

# see all compiler options at $NDK_PATH/toolchains/
export ANDROID_CROSS_COMPILER="arm-linux-androideabi-clang3.3"
export PLATFORM_PREFIX="${NDK_PATH}/platforms/android-19/"

# NOTE: make-standalone-toolchain.sh --help for options
${NDK_PATH}/build/tools/make-standalone-toolchain.sh  \
  --toolchain=${ANDROID_CROSS_COMPILER} \
  --install-dir="${PLATFORM_PREFIX}" \
  --stl=gnustl \
  --arch=arm \
  --platform="${API_LEVEL}"

ADT_BUNDLE="$(pwd)/adt-bundle-mac-x86_64-20131030"
PATH="${NDK_PATH}:${ADT_BUNDLE}/sdk/tools:${ADT_BUNDLE}/sdk/platform-tools":${PATH}
export PATH
echo $PATH
