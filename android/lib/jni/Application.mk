APP_MODULES := NativeMapView
APP_ABI := armeabi-v7a-hard
APP_PLATFORM := android-19
APP_STL := c++_static
APP_CPPFLAGS := -fexceptions -std=c++11

TARGET_CFLAGS += -mhard-float -D_NDK_MATH_NO_SOFTFP=1
TARGET_LDFLAGS += -Wl,--no-warn-mismatch -lm_hard

NDK_TOOLCHAIN_VERSION := clang

#http://blog.algolia.com/android-ndk-how-to-reduce-libs-size/
