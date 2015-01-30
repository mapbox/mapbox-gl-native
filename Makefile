BUILDTYPE ?= Release
PYTHON ?= python
PREFIX ?= /usr/local
JOBS ?= 1

ifeq ($(shell uname -s), Darwin)
DEFAULT_HOST ?= osx
endif
DEFAULT_HOST ?= linux

.PHONY: all
all: mbgl


#### Dependencies ##############################################################

config.gypi: configure
	./configure

config-ios.gypi: configure
	MASON_PLATFORM=ios ./configure config-ios.gypi

config-android.gypi: configure __android__/configure
__android__/configure:
	$(ENV) ./configure config-android.gypi


#### Configuration defaults ####################################################

__osx__/%: HOST ?= osx
__osx__/%: HEADLESS ?= cgl
__osx__/%: PLATFORM ?= osx
__osx__/%: ASSET ?= fs
__osx__/%: HTTP ?= nsurl
__osx__/%: CACHE ?= sqlite

__ios__/%: HOST = ios
__ios__/%: HEADLESS ?= none
__ios__/%: PLATFORM ?= ios
__ios__/%: ASSET ?= fs
__ios__/%: HTTP ?= nsurl
__ios__/%: CACHE ?= sqlite

__linux__/%: HOST = linux
__linux__/%: HEADLESS ?= glx
__linux__/%: PLATFORM ?= linux
__linux__/%: ASSET ?= fs
__linux__/%: HTTP ?= curl
__linux__/%: CACHE ?= sqlite

__android__/%: ENV=$(shell ./scripts/android_env.sh)
__android__/%: HOST = android
__android__/%: HEADLESS ?= none
__android__/%: PLATFORM ?= android
__android__/%: ASSET ?= zip
__android__/%: HTTP ?= curl
__android__/%: CACHE ?= sqlite

CONFIG_STRING += -Dhost=$(HOST)
CONFIG_STRING += -Dheadless_lib=$(HEADLESS)
CONFIG_STRING += -Dplatform_lib=$(PLATFORM)
CONFIG_STRING += -Dasset_lib=$(ASSET)
CONFIG_STRING += -Dhttp_lib=$(HTTP)
CONFIG_STRING += -Dcache_lib=$(CACHE)
CONFIG_STRING += -Dinstall_prefix=$(PREFIX)
CONFIG_STRING += --depth=.
CONFIG_STRING += -Goutput_dir=..


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
build/mbgl/Makefile: mbgl.gyp config.gypi __$(DEFAULT_HOST)__/mbgl
__%__/mbgl:
	deps/run_gyp mbgl.gyp -Iconfig.gypi $(CONFIG_STRING) --generator-output=./build/mbgl -f make

.PHONY: build/test/Makefile
build/test/Makefile: test/test.gyp config.gypi __$(DEFAULT_HOST)__/test
__%__/test:
	deps/run_gyp test/test.gyp -Iconfig.gypi $(CONFIG_STRING) --generator-output=./build/test -f make

.PHONY: build/linux/Makefile
build/linux/Makefile: linux/mapboxgl-app.gyp config.gypi __linux__/linux
__linux__/linux:
	deps/run_gyp linux/mapboxgl-app.gyp -Iconfig.gypi $(CONFIG_STRING) --generator-output=./build/linux -f make

.PHONY: build/macosx/Makefile
build/macosx/Makefile: macosx/mapboxgl-app.gyp config.gypi __osx__/osx
__osx__/osx:
	deps/run_gyp macosx/mapboxgl-app.gyp -Iconfig.gypi $(CONFIG_STRING) --generator-output=./build/macosx -f make

.PHONY: build/render/Makefile
build/render/Makefile: bin/render.gyp config.gypi __$(DEFAULT_HOST)__/render
__%__/render:
	deps/run_gyp bin/render.gyp -Iconfig.gypi $(CONFIG_STRING) --generator-output=./build/render -f make

.PHONY: build/test/test.xcodeproj
build/test/test.xcodeproj: test/test.gyp config.gypi __osx__/test-xcode
__osx__/test-xcode:
	deps/run_gyp test/test.gyp -Iconfig.gypi $(CONFIG_STRING) --generator-output=./build -f xcode

.PHONY: build/macosx/mapboxgl-app.xcodeproj
build/macosx/mapboxgl-app.xcodeproj: macosx/mapboxgl-app.gyp config.gypi __osx__/app-xcode
__osx__/app-xcode:
	deps/run_gyp macosx/mapboxgl-app.gyp -Iconfig.gypi $(CONFIG_STRING) --generator-output=./build -f xcode

.PHONY: build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj
build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj: ios/mapbox-gl-cocoa/app/mapboxgl-app.gyp config-ios.gypi __ios__/app-xcode
__ios__/app-xcode:
	deps/run_gyp ios/mapbox-gl-cocoa/app/mapboxgl-app.gyp -Iconfig-ios.gypi $(CONFIG_STRING) --generator-output=./build -f xcode

.PHONY: build/linux/mapboxgl-app.xcodeproj
build/linux/mapboxgl-app.xcodeproj: linux/mapboxgl-app.gyp config.gypi __linux__/app-xcode
__linux__/app-xcode:
	deps/run_gyp linux/mapboxgl-app.gyp -Iconfig.gypi $(CONFIG_STRING) --generator-output=./build -f xcode

.PHONY: build/bin/render.xcodeproj
build/bin/render.xcodeproj: bin/render.gyp config.gypi __$(HOST)__/render-xcode
__%__/render-xcode:
	deps/run_gyp bin/render.gyp -Iconfig.gypi $(CONFIG_STRING) --generator-output=./build -f xcode

.PHONY: build/android/Makefile
build/android/Makefile: android/mapboxgl-app.gyp config.gypi __android__/android-make
__android__/android-make:
	@echo deps/run_gyp android/mapboxgl-app.gyp -Iconfig-android.gypi $(CONFIG_STRING) --generator-output=./build/android -f make-android
	@$(ENV) deps/run_gyp android/mapboxgl-app.gyp -Iconfig-android.gypi $(CONFIG_STRING) --generator-output=./build/android -f make-android

.PHONY: android
android: __android__/android
__android__/android: build/android/Makefile
	@echo $(MAKE) -C build/android BUILDTYPE=$(BUILDTYPE) androidapp
	@$(ENV) $(MAKE) -C build/android BUILDTYPE=$(BUILDTYPE) androidapp
	mkdir -p android/java/lib/src/main/jniLibs/armeabi-v7a
	cp build/$(BUILDTYPE)/lib.target/libmapbox-gl.so android/java/lib/src/main/jniLibs/armeabi-v7a/libmapbox-gl.so
	mkdir -p android/java/lib/src/main/assets
	cp build/$(BUILDTYPE)/ca-bundle.crt android/java/lib/src/main/assets/ca-bundle.crt
	cp -r build/$(BUILDTYPE)/styles android/java/lib/src/main/assets/styles
	cd android/java && ./gradlew --parallel-threads=$(JOBS) build


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
	-rm -rf ./macosx/build/
	-rm -rf ./config.gypi ./config-ios.gypi ./config-android.gypi
	-rm -rf ./android/java/build ./android/java/app/build ./android/java/lib/build
	-rm -rf ./android/java/lib/src/main/jniLibs ./android/java/lib/src/main/assets
	-rm -f ./android/test/features.zip

distclean: clean
	-rm -rf ./mason_packages
