LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

NDK_TOOLCHAIN_VERSION=clang3.3

LOCAL_MODULE := llmr

LOCAL_C_INCLUDES += -I include

LOCAL_CPP_FEATURES := rtti exceptions
CXXFLAGS = -v -Wall -Wextra -std=c++11 -Wno-unused-parameter -latomic
LOCAL_LDLIBS := -llog -lGLESv2 -latomic
LOCAL_LDFLAGS += -DLINKER_DEBUG=1 -v

LOCAL_STATIC_LIBRARIES := png

# LOCAL_SRC_FILES := libpng.a
LOCAL_SRC_FILES    += request.cpp
LOCAL_SRC_FILES    += settings.cpp
LOCAL_SRC_FILES    += llmr.cpp
LOCAL_SRC_FILES    += $(wildcard src/*/*.cpp)
LOCAL_SRC_FILES    += $(wildcard src/libtess2/*.c)


include $(BUILD_SHARED_LIBRARY)
