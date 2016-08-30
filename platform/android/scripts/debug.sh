#!/usr/bin/env bash

set -e
set -o pipefail

# Automation of https://github.com/mapbox/mapbox-gl-native/wiki/Android-debugging-with-remote-GDB

export MASON_DIR="`pwd`/.mason"
export PATH="${MASON_DIR}:${PATH}"

export MASON_ANDROID_ABI=x86
export MASON_ANDROID_ARCH=x86
export MASON_ANDROID_PLATFORM=9
export MASON_NDK_PACKAGE_VERSION=${MASON_ANDROID_ARCH}-${MASON_ANDROID_PLATFORM}-r12b

if [[ $1 == '--prepare' ]]; then
  mkdir -p ~/.android/debugging/{vendor,system}_lib
  adb pull /system/lib ~/.android/debugging/system_lib
  adb pull /vendor/lib ~/.android/debugging/vendor_lib
  adb pull /system/bin/app_process ~/.android/debugging
  adb pull /system/bin/app_process32 ~/.android/debugging
  adb pull /system/bin/linker ~/.android/debugging

  if [[ ${MASON_ANDROID_ABI} == 'x86_64' || ${MASON_ANDROID_ABI} == 'mips64' ]]; then
    adb pull /system/bin/app_process64 ~/.android/debugging
    adb pull /system/bin/linker64 ~/.android/debugging
  fi

  cp `mason prefix android-ndk ${MASON_NDK_PACKAGE_VERSION}`/prebuilt/android-${MASON_ANDROID_ABI}/gdbserver/gdbserver \
     platform/android/MapboxGLAndroidSDK/src/main/jniLibs/${MASON_ANDROID_ABI}/gdbserver.so
fi

adb install -rtdg platform/android/MapboxGLAndroidSDKTestApp/build/outputs/apk/MapboxGLAndroidSDKTestApp-debug.apk
adb shell am start -n "com.mapbox.mapboxsdk.testapp/com.mapbox.mapboxsdk.testapp.activity.FeatureOverviewActivity" \
   -a android.intent.action.MAIN -c android.intent.category.LAUNCHER

adb forward tcp:5039 tcp:5039
adb shell run-as com.mapbox.mapboxsdk.testapp '/data/data/com.mapbox.mapboxsdk.testapp/lib/gdbserver.so \
   --attach :5039 `pgrep com.mapbox.mapboxsdk.testapp`' &

export MASON_PLATFORM=android
export PATH=`mason env PATH`
`mason env MASON_ANDROID_TOOLCHAIN`-gdb \
  -ex "target remote :5039" \
  -ex "set solib-search-path ~/.android/debugging:~/.android/debugging/system_lib:~/.android/debugging/vendor_lib:~/.android/debugging/vendor_lib/egl:./build/android-${MASON_ANDROID_ABI}/Debug/lib.target/"

