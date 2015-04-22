
#### Configuration defaults ####################################################

ENV_osx = MASON_PLATFORM=osx
CONFIG_osx = -Dhost=osx -Iconfig/osx.gypi -Dinstall_prefix=$(PREFIX)
LIBS_osx  = -Dheadless_lib=$(word 1,$(HEADLESS) cgl)
LIBS_osx += -Dplatform_lib=$(word 1,$(PLATFORM) osx)
LIBS_osx += -Dasset_lib=$(word 1,$(ASSET) fs)
LIBS_osx += -Dhttp_lib=$(word 1,$(HTTP) nsurl)
LIBS_osx += -Dcache_lib=$(word 1,$(CACHE) sqlite)
LIBS_osx += --depth=. -Goutput_dir=.


ENV_ios = MASON_PLATFORM=ios
CONFIG_ios = -Dhost=ios -Iconfig/ios.gypi -Dinstall_prefix=$(PREFIX)
LIBS_ios  = -Dheadless_lib=none
LIBS_ios += -Dplatform_lib=$(word 1,$(PLATFORM) ios)
LIBS_ios += -Dasset_lib=$(word 1,$(ASSET) fs)
LIBS_ios += -Dhttp_lib=$(word 1,$(HTTP) nsurl)
LIBS_ios += -Dcache_lib=$(word 1,$(CACHE) sqlite)
LIBS_ios += --depth=. -Goutput_dir=.


ENV_linux = MASON_PLATFORM=linux
CONFIG_linux = -Dhost=linux -Iconfig/linux.gypi -Dinstall_prefix=$(PREFIX)
LIBS_linux  = -Dheadless_lib=$(word 1,$(HEADLESS) glx)
LIBS_linux += -Dplatform_lib=$(word 1,$(PLATFORM) linux)
LIBS_linux += -Dasset_lib=$(word 1,$(ASSET) fs)
LIBS_linux += -Dhttp_lib=$(word 1,$(HTTP) curl)
LIBS_linux += -Dcache_lib=$(word 1,$(CACHE) sqlite)
LIBS_linux += --depth=. -Goutput_dir=.

ANDROID_ABIS += android-lib-arm-v8
ENV_android-arm-v8 = $(shell MASON_ANDROID_ABI=arm-v8 ./scripts/android_env.sh)
CONFIG_android-arm-v8 = -Dhost=android -Iconfig/android-arm-v8.gypi

ANDROID_ABIS += android-lib-arm-v7
ENV_android-arm-v7 = $(shell MASON_ANDROID_ABI=arm-v7 ./scripts/android_env.sh)
CONFIG_android-arm-v7 = -Dhost=android -Iconfig/android-arm-v7.gypi

ANDROID_ABIS += android-lib-arm-v5
ENV_android-arm-v5 = $(shell MASON_ANDROID_ABI=arm-v5 ./scripts/android_env.sh)
CONFIG_android-arm-v5 = -Dhost=android -Iconfig/android-arm-v5.gypi

ANDROID_ABIS += android-lib-x86
ENV_android-x86 = $(shell MASON_ANDROID_ABI=x86 ./scripts/android_env.sh)
CONFIG_android-x86 = -Dhost=android -Iconfig/android-x86.gypi

# OpenSSL build is incomplete.
# ANDROID_ABIS += android-lib-x86-64
# ENV_android-x86-64 = $(shell MASON_ANDROID_ABI=x86-64 ./scripts/android_env.sh)
# CONFIG_android-x86-64 = -Dhost=android -Iconfig/android-x86-64.gypi

ANDROID_ABIS += android-lib-mips
ENV_android-mips = $(shell MASON_ANDROID_ABI=mips ./scripts/android_env.sh)
CONFIG_android-mips = -Dhost=android -Iconfig/android-mips.gypi

ANDROID_ABIS += android-lib-mips-64
ENV_android-mips-64 = $(shell MASON_ANDROID_ABI=mips-64 ./scripts/android_env.sh)
CONFIG_android-mips-64 = -Dhost=android -Iconfig/android-mips-64.gypi

LIBS_android  = -Dheadless_lib=none
LIBS_android += -Dplatform_lib=$(word 1,$(PLATFORM) android)
LIBS_android += -Dasset_lib=$(word 1,$(ASSET) zip)
LIBS_android += -Dhttp_lib=$(word 1,$(HTTP) curl)
LIBS_android += -Dcache_lib=$(word 1,$(CACHE) sqlite)
LIBS_android += --depth=. -Goutput_dir=.
