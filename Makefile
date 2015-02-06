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

include config/defaults.mk

#### Dependencies ##############################################################

# Wildcard targets get removed after build by default, but we want to preserve the config.
.PRECIOUS: config/%.gypi
config/%.gypi: CMD = ./configure config/$*.gypi
config/%.gypi: configure
	@$(ENV_$*) ./scripts/flock.py build/Configure.lock ./configure config/$*.gypi

#### Library builds ############################################################

.PRECIOUS: Makefile/mbgl
Makefile/mbgl: config/$(HOST).gypi
	deps/run_gyp mbgl.gyp $(CONFIG_$(HOST)) $(LIBS_$(HOST)) --generator-output=./build/$(HOST) -f make

mbgl: Makefile/mbgl
	$(MAKE) -C build/$(HOST) BUILDTYPE=$(BUILDTYPE) everything

standalone: Makefile/mbgl
	LINK=`pwd`/gyp/link.py $(MAKE) -C build/$(HOST) BUILDTYPE=$(BUILDTYPE) standalone

install: Makefile/mbgl
	LINK=`pwd`/gyp/link.py $(MAKE) -C build/$(HOST) BUILDTYPE=$(BUILDTYPE) install

.PRECIOUS: Xcode/mbgl
Xcode/mbgl: config/$(HOST).gypi
	deps/run_gyp mbgl.gyp $(CONFIG_$(HOST)) $(LIBS_$(HOST)) --generator-output=./build/$(HOST) -f xcode

##### Test builds ##############################################################

.PRECIOUS: Makefile/test
Makefile/test: test/test.gyp config/$(HOST).gypi
	deps/run_gyp test/test.gyp $(CONFIG_$(HOST)) $(LIBS_$(HOST)) --generator-output=./build/$(HOST) -f make

test: Makefile/test
	$(MAKE) -C build/$(HOST) BUILDTYPE=$(BUILDTYPE) test

test-%: test
	./scripts/run_tests.sh "build/$(HOST)/$(BUILDTYPE)/test" --gtest_filter=$*


.PRECIOUS: Xcode/test
Xcode/test: test/test.gyp config/osx.gypi
	deps/run_gyp test/test.gyp $(CONFIG_osx) $(LIBS_osx) --generator-output=./build/osx -f xcode

.PHONY: lproj lbuild run-xlinux
xtest-proj: Xcode/test
	open ./build/osx/test/test.xcodeproj

xtest: Xcode/test
	xcodebuild -project ./build/osx/test/test.xcodeproj -configuration $(BUILDTYPE) -target test -jobs `sysctl -n hw.ncpu`

xtest-%: xtest
	./scripts/run_tests.sh "build/osx/Build/Products/$(BUILDTYPE)/test" --gtest_filter=$*


#### Mac OS X application builds ###############################################

.PRECIOUS: Makefile/osx
Makefile/osx: macosx/mapboxgl-app.gyp config/osx.gypi
	deps/run_gyp macosx/mapboxgl-app.gyp $(CONFIG_osx) $(LIBS_osx) --generator-output=./build/osx -f make

.PHONY: osx run-osx
osx: Makefile/osx
	$(MAKE) -C build/osx BUILDTYPE=$(BUILDTYPE) osxapp

run-osx: osx
	"build/osx/$(BUILDTYPE)/Mapbox GL.app/Contents/MacOS/Mapbox GL"


.PRECIOUS: Xcode/osx
Xcode/osx: macosx/mapboxgl-app.gyp config/osx.gypi
	deps/run_gyp macosx/mapboxgl-app.gyp $(CONFIG_osx) $(LIBS_osx) --generator-output=./build/osx -f xcode

.PHONY: xosx-proj xosx run-xosx
xosx-proj: Xcode/osx
	open ./build/osx/macosx/mapboxgl-app.xcodeproj

xosx: Xcode/osx
	xcodebuild -project ./build/osx/macosx/mapboxgl-app.xcodeproj -configuration $(BUILDTYPE) -target osxapp -jobs `sysctl -n hw.ncpu`

run-xosx: xosx
	"build/osx/Build/Products/$(BUILDTYPE)/Mapbox GL.app/Contents/MacOS/Mapbox GL"

# Legacy name
xproj: xosx-proj



#### iOS application builds ####################################################

.PRECIOUS: Xcode/ios
Xcode/ios: ios/app/mapboxgl-app.gyp config/ios.gypi
	deps/run_gyp ios/app/mapboxgl-app.gyp $(CONFIG_ios) $(LIBS_ios) --generator-output=./build/ios -f xcode

.PHONY: ios-proj ios run-ios
ios-proj: Xcode/ios
	open ./build/ios/ios/app/mapboxgl-app.xcodeproj

ios: Xcode/ios
	xcodebuild -sdk iphoneos ARCHS="arm64 armv7 armv7s" PROVISIONING_PROFILE="2b532944-bf3d-4bf4-aa6c-a81676984ae8" -project ./build/ios/ios/app/mapboxgl-app.xcodeproj -configuration Release -target iosapp -jobs `sysctl -n hw.ncpu`

isim: Xcode/ios
	xcodebuild -sdk iphonesimulator ARCHS="x86_64 i386" -project ./build/ios/ios/app/mapboxgl-app.xcodeproj -configuration Debug -target iosapp -jobs `sysctl -n hw.ncpu`

# Legacy name
iproj: ios-proj


#### Linux application builds ##################################################

.PRECIOUS: Makefile/linux
Makefile/linux: linux/mapboxgl-app.gyp config/$(HOST).gypi
	deps/run_gyp linux/mapboxgl-app.gyp $(CONFIG_$(HOST)) $(LIBS_linux) --generator-output=./build/$(HOST) -f make

.PHONY: linux run-linux
linux: Makefile/linux
	$(MAKE) -C build/$(HOST) BUILDTYPE=$(BUILDTYPE) linuxapp

run-linux: linux
	(cd build/$(HOST)/$(BUILDTYPE) && ./mapbox-gl)


.PRECIOUS: Xcode/linux
Xcode/linux: linux/mapboxgl-app.gyp config/osx.gypi
	deps/run_gyp linux/mapboxgl-app.gyp $(CONFIG_osx) $(LIBS_linux) --generator-output=./build/osx -f xcode

.PHONY: lproj lbuild run-xlinux
xlinux-proj: Xcode/linux
	open ./build/osx/linux/mapboxgl-app.xcodeproj

xlinux: Xcode/linux
	xcodebuild -project ./build/osx/linux/mapboxgl-app.xcodeproj -configuration $(BUILDTYPE) -target linuxapp

run-xlinux: xlinux
	"build/osx/Build/Products/$(BUILDTYPE)/mapbox-gl"

# Legacy name
lproj: xlinux-proj


#### Android libaries #########################################################

.PRECIOUS: Makefile/android-%
Makefile/android-%: CMD = deps/run_gyp android/mapboxgl-app.gyp $(CONFIG_android-$*) $(LIBS_android) --generator-output=./build/android-$* -f make-android
Makefile/android-%: config/android-%.gypi
	@echo $(CMD)
	@$(ENV_android-$*) $(CMD)

# Builds a particular android architecture.
android-lib-%: Makefile/android-%
	@$(ENV_android-$*) ./scripts/flock.py -v build/Android.lock $(MAKE) -C build/android-$* BUILDTYPE=$(BUILDTYPE) androidapp

# Builds the selected/default Android library
android-lib: android-lib-$(ANDROID_ABI)

# Builds the selected/default Android library
android: android-lib-$(ANDROID_ABI)
	cd android/java && ./gradlew --parallel-threads=$(JOBS) build

# Builds all android architectures.
android-all: $(ANDROID_ABIS)
	cd android/java && ./gradlew --parallel-threads=$(JOBS) build

# Full build of GL Core for each architecture, build the Android Library, publish to Maven Central
android-deploy: $(ANDROID_ABIS)
	cd android/java/MapboxGLAndroidSDK && chmod ugo+x deploy.sh && ./deploy.sh


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
