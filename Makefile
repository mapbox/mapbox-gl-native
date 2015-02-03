BUILDTYPE ?= Release
PYTHON ?= python
PREFIX ?= /usr/local
ANDROID_ABI ?= arm-v7
JOBS ?= 1

ifeq ($(shell uname -s), Darwin)
HOST ?= osx
endif
HOST ?= linux

all: mbgl

#### Configuration defaults ####################################################

ENV_osx = MASON_PLATFORM=osx
CONFIG_osx = -Dhost=osx -Iconfig-osx.gypi -Dinstall_prefix=$(PREFIX)
LIBS_osx  = -Dheadless_lib=$(word 1,$(HEADLESS) cgl)
LIBS_osx += -Dplatform_lib=$(word 1,$(PLATFORM) osx)
LIBS_osx += -Dasset_lib=$(word 1,$(ASSET) fs)
LIBS_osx += -Dhttp_lib=$(word 1,$(HTTP) nsurl)
LIBS_osx += -Dcache_lib=$(word 1,$(CACHE) sqlite)
LIBS_osx += --depth=. -Goutput_dir=.


ENV_ios = MASON_PLATFORM=ios
CONFIG_ios = -Dhost=ios -Iconfig-ios.gypi -Dinstall_prefix=$(PREFIX)
LIBS_ios  = -Dheadless_lib=none
LIBS_ios += -Dplatform_lib=$(word 1,$(PLATFORM) ios)
LIBS_ios += -Dasset_lib=$(word 1,$(ASSET) fs)
LIBS_ios += -Dhttp_lib=$(word 1,$(HTTP) nsurl)
LIBS_ios += -Dcache_lib=$(word 1,$(CACHE) sqlite)
LIBS_ios += --depth=. -Goutput_dir=.


ENV_linux = MASON_PLATFORM=linux
CONFIG_linux = -Dhost=linux -Iconfig-linux.gypi -Dinstall_prefix=$(PREFIX)
LIBS_linux  = -Dheadless_lib=$(word 1,$(HEADLESS) glx)
LIBS_linux += -Dplatform_lib=$(word 1,$(PLATFORM) linux)
LIBS_linux += -Dasset_lib=$(word 1,$(ASSET) fs)
LIBS_linux += -Dhttp_lib=$(word 1,$(HTTP) curl)
LIBS_linux += -Dcache_lib=$(word 1,$(CACHE) sqlite)
LIBS_linux += --depth=. -Goutput_dir=.

ANDROID_ABIS += android-lib-arm-v8
ENV_android-arm-v8 = $(shell MASON_ANDROID_ABI=arm-v8 ./scripts/android_env.sh)
CONFIG_android-arm-v8 = -Dhost=android -Iconfig-android-arm-v8.gypi

ANDROID_ABIS += android-lib-arm-v7
ENV_android-arm-v7 = $(shell MASON_ANDROID_ABI=arm-v7 ./scripts/android_env.sh)
CONFIG_android-arm-v7 = -Dhost=android -Iconfig-android-arm-v7.gypi

ANDROID_ABIS += android-lib-arm-v5
ENV_android-arm-v5 = $(shell MASON_ANDROID_ABI=arm-v5 ./scripts/android_env.sh)
CONFIG_android-arm-v5 = -Dhost=android -Iconfig-android-arm-v5.gypi

ANDROID_ABIS += android-lib-x86
ENV_android-x86 = $(shell MASON_ANDROID_ABI=x86 ./scripts/android_env.sh)
CONFIG_android-x86 = -Dhost=android -Iconfig-android-x86.gypi

# OpenSSL build is incomplete.
# ANDROID_ABIS += android-lib-x86-64
# ENV_android-x86-64 = $(shell MASON_ANDROID_ABI=x86-64 ./scripts/android_env.sh)
# CONFIG_android-x86-64 = -Dhost=android -Iconfig-android-x86-64.gypi

ANDROID_ABIS += android-lib-mips
ENV_android-mips = $(shell MASON_ANDROID_ABI=mips ./scripts/android_env.sh)
CONFIG_android-mips = -Dhost=android -Iconfig-android-mips.gypi

ANDROID_ABIS += android-lib-mips-64
ENV_android-mips-64 = $(shell MASON_ANDROID_ABI=mips-64 ./scripts/android_env.sh)
CONFIG_android-mips-64 = -Dhost=android -Iconfig-android-mips-64.gypi

LIBS_android  = -Dheadless_lib=none
LIBS_android += -Dplatform_lib=$(word 1,$(PLATFORM) android)
LIBS_android += -Dasset_lib=$(word 1,$(ASSET) zip)
LIBS_android += -Dhttp_lib=$(word 1,$(HTTP) curl)
LIBS_android += -Dcache_lib=$(word 1,$(CACHE) sqlite)
LIBS_android += --depth=. -Goutput_dir=.

#### Dependencies ##############################################################

# Wildcard targets get removed after build by default, but we want to preserve the config.
.PRECIOUS: config-%.gypi
config-%.gypi: CMD = ./configure config-$*.gypi
config-%.gypi: configure
	@echo $(CMD)
	@$(ENV_$*) $(CMD)
	cat config-$*.gypi

#### Library builds ############################################################

.PRECOIUS: Makefile/mbgl
Makefile/mbgl: config-$(HOST).gypi
	deps/run_gyp mbgl.gyp $(CONFIG_$(HOST)) $(LIBS_$(HOST)) --generator-output=./build/$(HOST) -f make

mbgl: Makefile/mbgl
	$(MAKE) -C build/$(HOST) BUILDTYPE=$(BUILDTYPE) everything

standalone: Makefile/mbgl
	LINK=`pwd`/gyp/link.py $(MAKE) -C build/$(HOST) BUILDTYPE=$(BUILDTYPE) standalone

install: Makefile/mbgl
	LINK=`pwd`/gyp/link.py $(MAKE) -C build/$(HOST) BUILDTYPE=$(BUILDTYPE) install


##### Test builds ##############################################################

.PRECIOUS: Makefile/test
Makefile/test: test/test.gyp config-$(HOST).gypi
	deps/run_gyp test/test.gyp $(CONFIG_$(HOST)) $(LIBS_$(HOST)) --generator-output=./build/$(HOST) -f make

test: Makefile/test
	$(MAKE) -C build/$(HOST) BUILDTYPE=$(BUILDTYPE) test

test-%: test
	./scripts/run_tests.sh "build/$(HOST)/$(BUILDTYPE)/test" --gtest_filter=$*


.PRECIOUS: Xcode/test
Xcode/test: test/test.gyp config-osx.gypi
	deps/run_gyp test/test.gyp $(CONFIG_osx) $(LIBS_osx) --generator-output=./build/osx -f xcode

.PHONY: lproj lbuild run-xlinux
xtest-proj: Xcode/test
	open ./build/osx/test/test.xcodeproj

xtest: Xcode/test
	xcodebuild -project ./build/osx/test/test.xcodeproj -configuration $(BUILDTYPE) -scheme test -derivedDataPath build/osx -jobs `sysctl -n hw.ncpu`

xtest-%: xtest
	./scripts/run_tests.sh "build/osx/Build/Products/$(BUILDTYPE)/test" --gtest_filter=$*


#### Mac OS X application builds ###############################################

.PRECIOUS: Makefile/osx
Makefile/osx: macosx/mapboxgl-app.gyp config-osx.gypi
	deps/run_gyp macosx/mapboxgl-app.gyp $(CONFIG_osx) $(LIBS_osx) --generator-output=./build/osx -f make

.PHONY: osx run-osx
osx: Makefile/osx
	$(MAKE) -C build/osx BUILDTYPE=$(BUILDTYPE) osxapp

run-osx: osx
	"build/osx/$(BUILDTYPE)/Mapbox GL.app/Contents/MacOS/Mapbox GL"


.PRECIOUS: Xcode/osx
Xcode/osx: macosx/mapboxgl-app.gyp config-osx.gypi
	deps/run_gyp macosx/mapboxgl-app.gyp $(CONFIG_osx) $(LIBS_osx) --generator-output=./build/osx -f xcode

.PHONY: xosx-proj xosx run-xosx
xosx-proj: Xcode/osx
	open ./build/osx/macosx/mapboxgl-app.xcodeproj

xosx: Xcode/osx
	xcodebuild -project ./build/osx/macosx/mapboxgl-app.xcodeproj -configuration $(BUILDTYPE) -scheme osxapp -derivedDataPath build/osx -jobs `sysctl -n hw.ncpu`

run-xosx: xosx
	"build/osx/Build/Products/$(BUILDTYPE)/Mapbox GL.app/Contents/MacOS/Mapbox GL"

# Legacy name
xproj: xosx-proj



#### iOS application builds ####################################################

.PRECIOUS: Xcode/ios
Xcode/ios: ios/mapbox-gl-cocoa/app/mapboxgl-app.gyp config-ios.gypi
	deps/run_gyp ios/mapbox-gl-cocoa/app/mapboxgl-app.gyp $(CONFIG_ios) $(LIBS_ios) --generator-output=./build/ios -f xcode

.PHONY: ios-proj ios run-ios
ios-proj: Xcode/ios
	open ./build/ios/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj

ios: Xcode/ios
	xcodebuild -sdk iphonesimulator ONLY_ACTIVE_ARCH=YES -project ./build/ios/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj -configuration $(BUILDTYPE) -scheme iosapp -derivedDataPath build/ios -jobs `sysctl -n hw.ncpu`

# Legacy name
iproj: ios-proj


#### Linux application builds ##################################################

.PRECIOUS: Makefile/linux
Makefile/linux: linux/mapboxgl-app.gyp config-$(HOST).gypi
	deps/run_gyp linux/mapboxgl-app.gyp $(CONFIG_$(HOST)) $(LIBS_linux) --generator-output=./build/$(HOST) -f make

.PHONY: linux run-linux
linux: Makefile/linux
	$(MAKE) -C build/$(HOST) BUILDTYPE=$(BUILDTYPE) linuxapp

run-linux: linux
	(cd build/$(HOST)/$(BUILDTYPE) && ./mapbox-gl)


.PRECIOUS: Xcode/linux
Xcode/linux: linux/mapboxgl-app.gyp config-osx.gypi
	deps/run_gyp linux/mapboxgl-app.gyp $(CONFIG_osx) $(LIBS_linux) --generator-output=./build/osx -f xcode

.PHONY: lproj lbuild run-xlinux
xlinux-proj: Xcode/linux
	open ./build/osx/linux/mapboxgl-app.xcodeproj

xlinux: Xcode/linux
	xcodebuild -project ./build/osx/linux/mapboxgl-app.xcodeproj -configuration $(BUILDTYPE) -scheme linuxapp -derivedDataPath build/osx

run-xlinux: xlinux
	"build/osx/Build/Products/$(BUILDTYPE)/mapbox-gl"

# Legacy name
lproj: xlinux-proj


##### Render application #######################################################
# .PHONY: build/render/Makefile
# build/render/Makefile: bin/render.gyp __$(HOST)__/render
# __%__/render: config-%.gypi
# 	deps/run_gyp bin/render.gyp -Iconfig-$*.gypi $(CONFIG_STRING) --generator-output=./build/render -f make

# .PHONY: build/bin/render.xcodeproj
# build/bin/render.xcodeproj: bin/render.gyp __osx__/render-xcode
# __osx__/render-xcode:config-osx.gypi
# 	deps/run_gyp bin/render.gyp -Iconfig-osx.gypi $(CONFIG_STRING) --generator-output=./build -f xcode

# # Builds the CLI render app
# render: build/render/Makefile
# 	$(MAKE) -C build/render BUILDTYPE=$(BUILDTYPE) mbgl-render

#### Android libaries #########################################################

.PRECIOUS: Makefile/android-%
Makefile/android-%: CMD = deps/run_gyp android/mapboxgl-app.gyp $(CONFIG_android-$*) $(LIBS_android) --generator-output=./build/android-$* -f make-android
Makefile/android-%: config-android-%.gypi
	@echo $(CMD)
	@$(ENV_android-$*) $(CMD)

# Builds all android architectures.
android-all: $(ANDROID_ABIS)
android-all: android

# Builds a particular android architecture.
android-lib-%: CMD = $(MAKE) -C build/android-$* BUILDTYPE=$(BUILDTYPE) androidapp
android-lib-%: Makefile/android-%
	@echo $(CMD)
	@$(ENV_android-$*) $(CMD)

# Builds the selected/default Android library
android: android-lib-$(ANDROID_ABI)
	cd android/java && ./gradlew --parallel-threads=$(JOBS) build

# rproj: build/bin/render.xcodeproj
# 	open ./build/bin/render.xcodeproj


##### Maintenace operations ####################################################

.PHONY: clear_xcode_cache
clear_xcode_cache:
ifeq ($(PLATFORM), osx)
	@CUSTOM_DD=`defaults read com.apple.dt.Xcode IDECustomDerivedDataLocation 2>/dev/null`; \
	if [ $$CUSTOM_DD ]; then \
		echo clearing files in $$CUSTOM_DD older than one day; \
		find $$CUSTOM_DD/mapboxgl-app-* -mtime +1 | xargs rm -rf; \
	fi; \
	if [ -d ~/Library/Developer/Xcode/DerivedData/ ] && [ ! $$CUSTOM_DD ]; then \
		echo 'clearing files in ~/Library/Developer/Xcode/DerivedData/mapboxgl-app-* older than one day'; \
		find ~/Library/Developer/Xcode/DerivedData/mapboxgl-app-* -mtime +1 | xargs rm -rf; \
	fi
endif

.PHONY: clear_sqlite_cache
clear_sqlite_cache:
ifeq ($(PLATFORM), osx)
	rm -f ~/Library/Application\ Support/Mapbox\ GL/cache.db
else
	rm -f /tmp/mbgl-cache.db
endif

clean: clear_sqlite_cache clear_xcode_cache
	-find ./deps/gyp -name "*.pyc" -exec rm {} \;
	-rm -rf ./build/
	-rm -rf ./config-*.gypi
	-rm -rf ./android/java/build ./android/java/app/build ./android/java/lib/build
	-rm -rf ./android/java/lib/src/main/jniLibs ./android/java/lib/src/main/assets
	-rm -f ./android/test/features.zip

distclean: clean
	-rm -rf ./mason_packages
