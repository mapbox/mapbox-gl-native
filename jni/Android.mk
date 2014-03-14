LOCAL_PATH := $(call my-dir)

include jni/libpng-android/jni/Application.mk

include $(CLEAR_VARS)

NDK_TOOLCHAIN_VERSION=clang3.3

LOCAL_MODULE := llmr

LOCAL_C_INCLUDES += -I include
LOCAL_C_INCLUDES += -I libpng-android

LOCAL_CPP_FEATURES := rtti exceptions
CXXFLAGS = -v -Wall -Wextra -std=c++11 -Wno-unused-parameter
LOCAL_LDLIBS := -llog -lGLESv2
LOCAL_LDFLAGS += -DLINKER_DEBUG=1 -v
LOCAL_STATIC_LIBRARIES := libpng
LOCAL_SHARED_LIBRARIES := libpng-android/obj/local/armeabi/libpng.a

LOCAL_SRC_FILES := libpng-android/obj/local/armeabi/libpng.a
LOCAL_SRC_FILES    += $(wildcard src/*/*.cpp)
LOCAL_SRC_FILES    += $(wildcard src/libtess2/*.c)

include $(BUILD_SHARED_LIBRARY)
