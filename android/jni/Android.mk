LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := MapboxGL
LOCAL_SRC_FILES := MapboxGL.cpp

include $(BUILD_SHARED_LIBRARY)
