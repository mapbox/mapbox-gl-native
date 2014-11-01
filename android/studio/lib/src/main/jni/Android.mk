LOCAL_PATH              := $(call my-dir)
MBGL_ROOT               := ../../..
MBGL_INC_ROOT           := ../..
BUILDTYPE               ?= Release
MBGL_OUT                := $(MBGL_ROOT)/build/$(BUILDTYPE)/obj
MBGL_OUT_TARGET         := $(MBGL_ROOT)/build/$(BUILDTYPE)/obj.target
MAPNIK_PACKAGING_OUT    := /mapnik-packaging/osx/out/build-cpp11-libcpp-gcc-arm-android
MAPNIK_PACKAGING_LIB    := $(MBGL_ROOT)$(MAPNIK_PACKAGING_OUT)/lib
MAPNIK_PACKAGING_INC    := $(MBGL_INC_ROOT)$(MAPNIK_PACKAGING_OUT)/include

#include $(CLEAR_VARS)
#LOCAL_MODULE            := boost_regex
#LOCAL_SRC_FILES         := $(MAPNIK_PACKAGING_LIB)/libboost_regex.a
#LOCAL_EXPORT_C_INCLUDES := $(MAPNIK_PACKAGING_INC)
#include $(PREBUILT_STATIC_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE            := boost_atomic
#LOCAL_SRC_FILES         := $(MAPNIK_PACKAGING_LIB)/libboost_atomic.a
#LOCAL_EXPORT_C_INCLUDES := $(MAPNIK_PACKAGING_INC)
#include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := png
LOCAL_SRC_FILES         := $(MAPNIK_PACKAGING_LIB)/libpng.a
LOCAL_EXPORT_C_INCLUDES := $(MAPNIK_PACKAGING_INC)
LOCAL_EXPORT_LDLIBS     := -lz
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := uv
LOCAL_SRC_FILES         := $(MAPNIK_PACKAGING_LIB)/libuv.a
LOCAL_EXPORT_C_INCLUDES := $(MAPNIK_PACKAGING_INC)
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := crypto
LOCAL_SRC_FILES         := $(MAPNIK_PACKAGING_LIB)/libcrypto.a
LOCAL_EXPORT_C_INCLUDES := $(MAPNIK_PACKAGING_INC)
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := ssl
LOCAL_SRC_FILES         := $(MAPNIK_PACKAGING_LIB)/libssl.a
LOCAL_EXPORT_C_INCLUDES := $(MAPNIK_PACKAGING_INC)
LOCAL_STATIC_LIBRARIES  := crypto
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := curl
LOCAL_SRC_FILES         := $(MAPNIK_PACKAGING_LIB)/libcurl.a
LOCAL_EXPORT_C_INCLUDES := $(MAPNIK_PACKAGING_INC)
LOCAL_EXPORT_LDLIBS     := -lz
LOCAL_STATIC_LIBRARIES  := ssl
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := sqlite
LOCAL_SRC_FILES         := $(MAPNIK_PACKAGING_LIB)/libsqlite3.a
LOCAL_EXPORT_C_INCLUDES := $(MAPNIK_PACKAGING_INC)
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := mbgl
LOCAL_SRC_FILES         := $(MBGL_OUT_TARGET)/libmapboxgl.a
LOCAL_EXPORT_C_INCLUDES := $(MBGL_INC_ROOT)/include
LOCAL_EXPORT_LDLIBS     := -latomic
LOCAL_STATIC_LIBRARIES  := png uv sqlite
# boost_regex
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := NativeMapView
LOCAL_SRC_FILES         := NativeMapView.cpp \
                           JNI.cpp \
                           $(MBGL_ROOT)/common/http_request_baton_curl.cpp \
                           $(MBGL_ROOT)/common/platform_default.cpp \
                           $(MBGL_ROOT)/common/linux.cpp
# TODO need to add android specific platform stuff for case and cache db
LOCAL_STATIC_LIBRARIES  := mbgl curl
# boost_atomic
LOCAL_LDLIBS            := -llog -landroid -lEGL -lGLESv2
include $(BUILD_SHARED_LIBRARY)
