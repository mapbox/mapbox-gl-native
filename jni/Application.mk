APP_STL      = gnustl_static
APP_CFLAGS += -DU_HAVE_STD_STRING
APP_CPPFLAGS += -DU_HAVE_STD_STRING -fexceptions -std=c++11
NDK_TOOLCHAIN_VERSION=4.8
APP_PLATFORM = android-19
LOCAL_LDFLAGS += -DLINKER_DEBUG=1 -v

# TODO - do
#APP_CPPFLAGS += -D__GXX_EXPERIMENTAL_CXX0X__
# or
#APP_USE_CPP0X := true
# do anything? seen them around...