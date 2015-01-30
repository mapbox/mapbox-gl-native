BUILDTYPE ?= Release
PYTHON ?= python
PREFIX ?= /usr/local
JOBS ?= 1

ifeq ($(shell uname -s), Darwin)
DEFAULT_HOST ?= osx
endif
DEFAULT_HOST ?= linux

ANDROID_ABI ?= arm-v7

.PHONY: all
all: mbgl

#### Configuration defaults ####################################################

__osx__/%: ENV = MASON_PLATFORM=osx
__osx__/%: HOST ?= osx
__osx__/%: HEADLESS ?= cgl
__osx__/%: PLATFORM ?= osx
__osx__/%: ASSET ?= fs
__osx__/%: HTTP ?= nsurl
__osx__/%: CACHE ?= sqlite

__ios__/%: ENV = MASON_PLATFORM=ios
__ios__/%: HOST = ios
__ios__/%: HEADLESS ?= none
__ios__/%: PLATFORM ?= ios
__ios__/%: ASSET ?= fs
__ios__/%: HTTP ?= nsurl
__ios__/%: CACHE ?= sqlite

__linux__/%: ENV = MASON_PLATFORM=linux
__linux__/%: HOST = linux
__linux__/%: HEADLESS ?= glx
__linux__/%: PLATFORM ?= linux
__linux__/%: ASSET ?= fs
__linux__/%: HTTP ?= curl
__linux__/%: CACHE ?= sqlite

__android-%__/gyp: ENV = $(shell MASON_ANDROID_ABI=$* ./scripts/android_env.sh)
__android-%__/build: ENV = $(shell MASON_ANDROID_ABI=$* ./scripts/android_env.sh)

__android-%__/gyp: HOST = android
__android-%__/gyp: HEADLESS ?= none
__android-%__/gyp: PLATFORM ?= android
__android-%__/gyp: ASSET ?= zip
__android-%__/gyp: HTTP ?= curl
__android-%__/gyp: CACHE ?= sqlite

CONFIG_STRING += -Dhost=$(HOST)
CONFIG_STRING += -Dheadless_lib=$(HEADLESS)
CONFIG_STRING += -Dplatform_lib=$(PLATFORM)
CONFIG_STRING += -Dasset_lib=$(ASSET)
CONFIG_STRING += -Dhttp_lib=$(HTTP)
CONFIG_STRING += -Dcache_lib=$(CACHE)
CONFIG_STRING += -Dinstall_prefix=$(PREFIX)
CONFIG_STRING += --depth=.
CONFIG_STRING += -Goutput_dir=..

#### Dependencies ##############################################################

.PHONY: FORCE
FORCE:

.PRECIOUS: config-%.gypi
config-%.gypi: configure __%__/configure
	cat config-$*.gypi

__%__/configure:
	$(ENV) ./configure config-$*.gypi

#### Library builds ############################################################

.PHONY: mbgl
mbgl: build/mbgl/Makefile
	$(MAKE) -C build/mbgl BUILDTYPE=$(BUILDTYPE) everything

.PHONY: install
install: build/mbgl/Makefile
	LINK=`pwd`/gyp/link.py $(MAKE) -C build/mbgl BUILDTYPE=$(BUILDTYPE) install

.PHONY: standalone
standalone: build/mbgl/Makefile
	LINK=`pwd`/gyp/link.py $(MAKE) -C build/mbgl BUILDTYPE=$(BUILDTYPE) standalone


#### Application buidls ########################################################

.PHONY: build/mbgl/Makefile
build/mbgl/Makefile: mbgl.gyp __$(DEFAULT_HOST)__/mbgl
__%__/mbgl: config-%.gypi
	deps/run_gyp mbgl.gyp -Iconfig-$*.gypi $(CONFIG_STRING) --generator-output=./build/mbgl -f make

.PHONY: build/test/Makefile
build/test/Makefile: test/test.gyp __$(DEFAULT_HOST)__/test
__%__/test: config-%.gypi
	deps/run_gyp test/test.gyp -Iconfig-$*.gypi $(CONFIG_STRING) --generator-output=./build/test -f make

.PHONY: build/linux/Makefile
build/linux/Makefile: linux/mapboxgl-app.gyp __linux__/linux
__linux__/linux: config-linux.gypi
	deps/run_gyp linux/mapboxgl-app.gyp -Iconfig-linux.gypi $(CONFIG_STRING) --generator-output=./build/linux -f make

.PHONY: build/macosx/Makefile
build/macosx/Makefile: macosx/mapboxgl-app.gyp __osx__/osx
__osx__/osx: config-osx.gypi
	deps/run_gyp macosx/mapboxgl-app.gyp -Iconfig-osx.gypi $(CONFIG_STRING) --generator-output=./build/macosx -f make

.PHONY: build/render/Makefile
build/render/Makefile: bin/render.gyp __$(DEFAULT_HOST)__/render
__%__/render: config-%.gypi
	deps/run_gyp bin/render.gyp -Iconfig-$*.gypi $(CONFIG_STRING) --generator-output=./build/render -f make

.PHONY: build/test/test.xcodeproj
build/test/test.xcodeproj: test/test.gyp __osx__/test-xcode
__osx__/test-xcode: config-osx.gypi
	deps/run_gyp test/test.gyp -Iconfig-osx.gypi $(CONFIG_STRING) --generator-output=./build -f xcode

.PHONY: build/macosx/mapboxgl-app.xcodeproj
build/macosx/mapboxgl-app.xcodeproj: macosx/mapboxgl-app.gyp __osx__/app-xcode
__osx__/app-xcode: config-osx.gypi
	deps/run_gyp macosx/mapboxgl-app.gyp -Iconfig-osx.gypi $(CONFIG_STRING) --generator-output=./build -f xcode

.PHONY: build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj
build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj: ios/mapbox-gl-cocoa/app/mapboxgl-app.gyp __ios__/app-xcode
__ios__/app-xcode: config-ios.gypi
	deps/run_gyp ios/mapbox-gl-cocoa/app/mapboxgl-app.gyp -Iconfig-ios.gypi $(CONFIG_STRING) -Goutput_dir=. --generator-output=./build -f xcode

.PHONY: build/linux/mapboxgl-app.xcodeproj
build/linux/mapboxgl-app.xcodeproj: linux/mapboxgl-app.gyp __linux__/app-xcode
__linux__/app-xcode: config-osx.gypi
	deps/run_gyp linux/mapboxgl-app.gyp -Iconfig-osx.gypi $(CONFIG_STRING) --generator-output=./build -f xcode

.PHONY: build/bin/render.xcodeproj
build/bin/render.xcodeproj: bin/render.gyp __osx__/render-xcode
__osx__/render-xcode:config-osx.gypi
	deps/run_gyp bin/render.gyp -Iconfig-osx.gypi $(CONFIG_STRING) --generator-output=./build -f xcode

build/android-%/Makefile: android/mapboxgl-app.gyp __android-%__/gyp
	@# leave this line here

__android-%__/gyp: GYP_CMD = deps/run_gyp android/mapboxgl-app.gyp -Iconfig-android-$*.gypi $(CONFIG_STRING) -Goutput_dir=. --generator-output=./build/android-$* -f make-android
__android-%__/gyp: config-android-%.gypi
	@echo $(GYP_CMD) ; $(ENV) $(GYP_CMD)

__android-%__/build: MAKE_CMD = $(MAKE) -C build/android-$* BUILDTYPE=$(BUILDTYPE) androidapp
__android-%__/build: build/android-%/Makefile
	@echo $(MAKE_CMD) ; $(ENV) $(MAKE_CMD)


##### Test cases ###############################################################

test: build/test/Makefile
	$(MAKE) -C build/test BUILDTYPE=$(BUILDTYPE) test

test-%: test
	./scripts/run_tests.sh --gtest_filter=$*

# build Mac OS X project for Xcode
xtest: build/test/test.xcodeproj
	open ./build/test/test.xcodeproj


##### Makefile builds ##########################################################

# Builds the linux app with make.
linux: build/linux/Makefile
	$(MAKE) -C build/linux BUILDTYPE=$(BUILDTYPE) linuxapp

# Executes the Linux binary
run-linux: linux
	(cd build/$(BUILDTYPE) && ./mapbox-gl)

# Builds the OS X app with make.
osx: build/macosx/Makefile
	$(MAKE) -C build/macosx BUILDTYPE=$(BUILDTYPE) osxapp

# Executes the OS X binary
run-osx: osx
	build/$(BUILDTYPE)/Mapbox\ GL.app/Contents/MacOS/MAPBOX\ GL

# Builds the iOS app with Xcode.
ios: build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj
	xcodebuild -project ./build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj -sdk iphonesimulator ONLY_ACTIVE_ARCH=NO -jobs $JOBS

# Builds the selected/default Android library
android: __android-$(ANDROID_ABI)__/build
	cd android/java && ./gradlew --parallel-threads=$(JOBS) build

# Builds all android architectures. As new architectures appear, add them to this list.
android-all: __android-arm-v7__/build
android-all: __android-arm-v8__/build
android-all: android

# Builds the CLI render app
render: build/render/Makefile
	$(MAKE) -C build/render BUILDTYPE=$(BUILDTYPE) mbgl-render

##### Xcode projects ###########################################################

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

xproj: build/macosx/mapboxgl-app.xcodeproj
	open ./build/macosx/mapboxgl-app.xcodeproj

iproj: build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj
	open ./build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj

rproj: build/bin/render.xcodeproj
	open ./build/bin/render.xcodeproj

# build Linux project for Xcode (Runs on Mac OS X too, but without platform-specific code)
lproj: build/linux/mapboxgl-app.xcodeproj
	open ./build/linux/mapboxgl-app.xcodeproj


##### Maintenace operations ####################################################

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
