BUILDTYPE ?= Release
PYTHON ?= python
PREFIX ?= /usr/local
ANDROID_ABI ?= arm-v7

ifeq ($(shell uname -s), Darwin)
HOST = osx
HEADLESS = cgl
JOBS ?= $(shell sysctl -n hw.ncpu)
endif

ifeq ($(shell uname -s), Linux)
HOST = linux
JOBS ?= $(shell grep --count processor /proc/cpuinfo)
BUILD_ROOT = $(shell pwd)
endif

JOBS ?= 1

all: mbgl

include config/defaults.mk

#### Dependencies ##############################################################

# Wildcard targets get removed after build by default, but we want to preserve the config.
.PRECIOUS: config/%.gypi
config/%.gypi: CMD = ./configure config/$*.gypi
config/%.gypi: configure
	@$(ENV_$*) ./scripts/flock.py build/Configure.lock ./configure config/$*.gypi

styles/styles:
	git submodule update --init styles

ifeq ($(shell uname -s), Darwin)
SMCalloutView:
	git submodule update --init platform/ios/vendor/SMCalloutView
else
SMCalloutView:
endif

geojsonvt:
	git submodule update --init src/mbgl/util/geojsonvt

KIF:
	git submodule update --init test/ios/KIF


#### Build files ###############################################################

.PRECIOUS: Makefile/project
Makefile/project: config/$(HOST).gypi styles/styles SMCalloutView geojsonvt
	deps/run_gyp gyp/$(HOST).gyp $(CONFIG_$(HOST)) $(LIBS_$(HOST)) --generator-output=./build/$(HOST) -f make

.PRECIOUS: Xcode/project
Xcode/project: config/$(HOST).gypi styles/styles SMCalloutView geojsonvt
	deps/run_gyp gyp/$(HOST).gyp $(CONFIG_$(HOST)) $(LIBS_$(HOST)) --generator-output=./build/$(HOST) -f xcode


#### Library builds ############################################################

mbgl: Makefile/project
	$(MAKE) -C build/$(HOST) BUILDTYPE=$(BUILDTYPE) everything

standalone: Makefile/project
	LINK=`pwd`/gyp/link.py $(MAKE) -C build/$(HOST) BUILDTYPE=$(BUILDTYPE) standalone

install: Makefile/project
	LINK=`pwd`/gyp/link.py $(MAKE) -C build/$(HOST) BUILDTYPE=$(BUILDTYPE) install


##### Test builds ##############################################################

.PHONY: test
test: Makefile/project
	$(MAKE) -C build/$(HOST) BUILDTYPE=$(BUILDTYPE) test

test-%: test
	./scripts/run_tests.sh "build/$(HOST)/$(BUILDTYPE)/test" --gtest_filter=$*

.PHONY: xtest
xtest: XCPRETTY := $(shell ./scripts/xcpretty.sh)
xtest: Xcode/project
	xcodebuild -project ./build/osx/gyp/osx.xcodeproj -configuration $(BUILDTYPE) -target test -jobs $(JOBS) $(XCPRETTY)

xtest-%: xtest
	./scripts/run_tests.sh "build/osx/Build/Products/$(BUILDTYPE)/test" --gtest_filter=$*


#### Mac OS X application builds ###############################################

.PHONY: osx run-osx
osx: Makefile/project
	$(MAKE) -C build/osx BUILDTYPE=$(BUILDTYPE) osxapp

run-osx: osx
	"build/osx/$(BUILDTYPE)/Mapbox GL.app/Contents/MacOS/Mapbox GL"

run-valgrind-osx: osx
	(valgrind --leak-check=full --suppressions=scripts/valgrind.sup build/osx/$(BUILDTYPE)/Mapbox\ GL.app/Contents/MacOS/Mapbox\ GL)

.PHONY: xosx-proj xosx run-xosx
xosx-proj: Xcode/project
	open ./build/osx/gyp/osx.xcodeproj

xosx: XCPRETTY := $(shell ./scripts/xcpretty.sh)
xosx: Xcode/project
	xcodebuild -project ./build/osx/gyp/osx.xcodeproj -configuration $(BUILDTYPE) -target osxapp -jobs $(JOBS) $(XCPRETTY)

run-xosx: xosx
	"gyp/build/$(BUILDTYPE)/Mapbox GL.app/Contents/MacOS/Mapbox GL"

# Legacy name
xproj: xosx-proj



#### iOS application builds ####################################################

.PRECIOUS: Xcode/ios
Xcode/ios: gyp/ios.gyp config/ios.gypi styles/styles SMCalloutView geojsonvt
	deps/run_gyp gyp/ios.gyp $(CONFIG_ios) $(LIBS_ios) --generator-output=./build/ios -f xcode

.PHONY: ios-proj ios isim ipackage
ios-proj: Xcode/ios
	open ./build/ios/gyp/ios.xcodeproj

ios: XCPRETTY := $(shell ./scripts/xcpretty.sh)
ios: Xcode/ios
	xcodebuild -sdk iphoneos ARCHS="arm64 armv7 armv7s" PROVISIONING_PROFILE="19324a54-7455-4f0b-8e1c-e6957c718ebc" -project ./build/ios/gyp/ios.xcodeproj -configuration Release -target iosapp -jobs $(JOBS) $(XCPRETTY)

ibench: XCPRETTY := $(shell ./scripts/xcpretty.sh)
ibench: Xcode/ios
	xcodebuild -sdk iphoneos ARCHS="arm64" PROVISIONING_PROFILE="19324a54-7455-4f0b-8e1c-e6957c718ebc" -project ./build/ios/gyp/ios.xcodeproj -configuration Release -target ios-bench -jobs $(JOBS) $(XCPRETTY)

isim: XCPRETTY := $(shell ./scripts/xcpretty.sh)
isim: Xcode/ios
	xcodebuild -sdk iphonesimulator ARCHS="x86_64 i386" -project ./build/ios/gyp/ios.xcodeproj -configuration Debug -target iosapp -jobs $(JOBS) $(XCPRETTY)

ipackage: Xcode/ios
	JOBS=$(JOBS) ./scripts/ios/package.sh

ipackage-strip: Xcode/ios
	JOBS=$(JOBS) ./scripts/ios/package.sh strip

ipackage-sim: Xcode/ios
	JOBS=$(JOBS) ./scripts/ios/package.sh sim

itest: ipackage-sim KIF
	./scripts/ios/test.sh

# Legacy name
iproj: ios-proj


#### Linux application builds ##################################################

.PHONY: linux run-linux
linux: Makefile/project
	$(MAKE) -C build/$(HOST) BUILDTYPE=$(BUILDTYPE) linuxapp

run-linux: linux
	(cd build/$(HOST)/$(BUILDTYPE) && ./mapbox-gl)

run-valgrind-linux: linux
	(cd build/$(HOST)/$(BUILDTYPE) && valgrind --leak-check=full --suppressions=$(BUILD_ROOT)/scripts/valgrind.sup ./mapbox-gl)

.PHONY: xlinux run-xlinux
xlinux: XCPRETTY := $(shell ./scripts/xcpretty.sh)
xlinux: Xcode/project
	xcodebuild -project ./build/osx/gyp/osx.xcodeproj -configuration $(BUILDTYPE) -target linuxapp -jobs $(JOBS) $(XCPRETTY)

run-xlinux: xlinux
	"gyp/build/$(BUILDTYPE)/mapbox-gl"


#### Android libaries #########################################################

.PRECIOUS: Makefile/android-%
Makefile/android-%: CMD = deps/run_gyp android/mapboxgl-app.gyp $(CONFIG_android-$*) $(LIBS_android) --generator-output=./build/android-$* -f make-android
Makefile/android-%: config/android-%.gypi styles/styles geojsonvt
	@echo $(CMD)
	@$(ENV_android-$*) $(CMD)

# Builds a particular android architecture.
android-lib-%: Makefile/android-%
	@$(ENV_android-$*) ./scripts/flock.py -v build/Android.lock $(MAKE) -C build/android-$* BUILDTYPE=$(BUILDTYPE) androidapp

# Builds the selected/default Android library
android-lib: android-lib-$(ANDROID_ABI)

# Builds the selected/default Android library
android: android-lib-$(ANDROID_ABI)
	cd android/java && ./gradlew --parallel-threads=$(JOBS) assemble$(BUILDTYPE)

# Builds all android architectures.
android-all: $(ANDROID_ABIS)
	cd android/java && ./gradlew --parallel-threads=$(JOBS) assemble$(BUILDTYPE)

# Full build of GL Core for each architecture, build the Android Library, publish to Maven Central
android-deploy: $(ANDROID_ABIS)
	cd android/java/MapboxGLAndroidSDK && chmod ugo+x deploy.sh && ./deploy.sh

android-project: android-lib


##### Render builds ############################################################

render: Makefile/project
	$(MAKE) -C build/$(HOST) BUILDTYPE=$(BUILDTYPE) mbgl-render

.PHONY: xrender
xrender: XCPRETTY := $(shell ./scripts/xcpretty.sh)
xrender: Xcode/project
	xcodebuild -project ./build/osx/gyp/osx.xcodeproj -configuration $(BUILDTYPE) -target mbgl-render -jobs $(JOBS) $(XCPRETTY)


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
	-rm -rf ./macosx/build
	-rm -rf ./linux/build
	-rm -rf ./ios/build
	-rm -rf ./test/build
	-rm -rf ./config/*.gypi
	-rm -rf ./android/java/build ./android/java/MapboxGLAndroidSDKTestApp/build ./android/java/MapboxGLAndroidSDKTestApp/build
	-rm -rf ./android/java/MapboxGLAndroidSDK/src/main/jniLibs ./android/java/MapboxGLAndroidSDK/src/main/assets
	-rm -f ./android/test/features.zip

distclean: clean
	-rm -rf ./mason_packages
