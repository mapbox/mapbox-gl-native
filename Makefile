export BUILDTYPE ?= Release
export BUILD_TEST ?= 1
export BUILD_RENDER ?= 1

# Determine build platform
ifeq ($(shell uname -s), Darwin)
export BUILD = osx
export JOBS ?= $(shell sysctl -n hw.ncpu)
export XCPRETTY := $(shell ./scripts/xcpretty.sh)
else ifeq ($(shell uname -s), Linux)
export BUILD = linux
export JOBS ?= $(shell grep --count processor /proc/cpuinfo)
else
$(error Cannot determine build platform)
endif
export BUILD_VERSION = $(shell uname -m)

RUN =  +@$(MAKE) -f scripts/main.mk

default: ; @printf "You must specify a valid target\n"

#### OS X targets ##############################################################

ifeq ($(BUILD),osx)
.PHONY: osx xosx nosx run-osx run-xosx
osx: ; $(RUN) HOST=osx HOST_VERSION=x86_64 Makefile/osxapp
xosx: ; $(RUN) HOST=osx HOST_VERSION=x86_64 Xcode/osxapp
nosx: ; $(RUN) HOST=osx HOST_VERSION=x86_64 Ninja/osxapp
run-osx: osx ; @"build/osx-x86_64/$(BUILDTYPE)/Mapbox GL.app/Contents/MacOS/Mapbox GL"
run-xosx: xosx ; @"gyp/build/$(BUILDTYPE)/Mapbox GL.app/Contents/MacOS/Mapbox GL"

.PHONY: Xcode/osx Xcode/ios
Xcode/ios: ; $(RUN) HOST=ios Xcode/__project__
Xcode/osx: ; $(RUN) HOST=osx HOST_VERSION=x86_64 Xcode/__project__

.PHONY: xproj iproj
xproj: Xcode/osx ; @open ./build/osx-x86_64/gyp/osx.xcodeproj
iproj: Xcode/ios ; @open ./build/ios-all/gyp/ios.xcodeproj

.PHONY: ios ibench isim
ios ibench isim: export XCODEBUILD_ARGS += PROVISIONING_PROFILE="$$(PROVISIONING_PROFILE)"

ios: export XCODEBUILD_ARGS += -sdk iphoneos ARCHS="arm64 armv7 armv7s"
ios: ; $(RUN) HOST=ios Xcode/iosapp

isim: export XCODEBUILD_ARGS += -sdk iphonesimulator ARCHS="x86_64 i386"
isim: ; $(RUN) HOST=ios Xcode/iosapp

ibench: export XCODEBUILD_ARGS += -sdk iphoneos ARCHS="arm64"
ibench: ; $(RUN) HOST=ios Xcode/ios-bench

.PHONY: ipackage ipackage-strip ipackage-sim ipackage-no-bitcode itest
ipackage: Xcode/ios ; @JOBS=$(JOBS) ./scripts/ios/package.sh
ipackage-strip: Xcode/ios ; @JOBS=$(JOBS) ./scripts/ios/package.sh strip
ipackage-sim: Xcode/ios ; @JOBS=$(JOBS) ./scripts/ios/package.sh sim
ipackage-no-bitcode: Xcode/ios ; @JOBS=$(JOBS) ./scripts/ios/package.sh no-bitcode
iframework: ipackage-strip ; ./scripts/ios/framework.sh
itest: ipackage-sim ; ./scripts/ios/test.sh

.PHONY: xpackage xpackage-strip
xpackage: Xcode/osx ; @JOBS=$(JOBS) ./scripts/osx/package.sh
xpackage-strip: Xcode/osx ; @JOBS=$(JOBS) ./scripts/osx/package.sh strip
endif

#### All platforms targets #####################################################

.PHONY: linux run-linux run-valgrind-linux
linux: ; $(RUN) Makefile/linuxapp
nlinux: ; $(RUN) Ninja/linuxapp
run-linux: linux ; (cd build/$(BUILD)-$(BUILD_VERSION)/$(BUILDTYPE) && ./mapbox-gl)
run-valgrind-linux: linux
	(cd build/$(BUILD)-$(BUILD_VERSION)/$(BUILDTYPE) && valgrind --leak-check=full --suppressions=../../../scripts/valgrind.sup ./mapbox-gl)


.PHONY: config compdb tidy
config: ; $(RUN) config
# Generates a compilation database with ninja for use in clang tooling
compdb: ; $(RUN) Ninja/compdb
tidy: ; $(RUN) tidy

.PHONY: android android-lib
# Builds a particular android architecture.
android-lib-%: ; $(RUN) HOST=android HOST_VERSION=$* Makefile/androidapp

# Builds the default Android library
android-lib: ; $(RUN) HOST=android Makefile/androidapp

# Builds the selected/default Android library
android: android-lib
	cd platform/android && ./gradlew --parallel --max-workers=$(JOBS) assemble$(BUILDTYPE)

# Builds all android architectures for distribution.
apackage: android-lib-arm-v5 android-lib-arm-v7
apackage: android-lib-x86
apackage: android-lib-mips
	cd platform/android && ./gradlew --parallel-threads=$(JOBS) assemble$(BUILDTYPE)

# Builds the Node.js library
.PHONY: node
node: ; $(RUN) HTTP=none ASSET=none CACHE=none Makefile/node


.PHONY: Xcode/node
Xcode/node: ; $(RUN) HTTP=none ASSET=none CACHE=none Xcode/node

.PHONY: xnode
xnode: Xcode/node ; @open ./build/binding.xcodeproj
nproj:
	$(RUN) HTTP=none ASSET=none CACHE=none node/xproj
	@open ./build/binding.xcodeproj

.PHONY: test
test: ; $(RUN) Makefile/test
test-%: ; $(RUN) test-$*
ifeq ($(BUILD),osx)
xtest: ; $(RUN) HOST=osx HOST_VERSION=x86_64 Xcode/test
endif

.PHONY: render xrender
render: ; $(RUN) Makefile/mbgl-render
ifeq ($(BUILD),osx)
xrender: ; $(RUN) HOST=osx HOST_VERSION=x86_64 Xcode/mbgl-render
endif


##### Maintenace operations ####################################################

.PHONY: clear_xcode_cache
clear_xcode_cache:
ifeq ($(BUILD), osx)
	@CUSTOM_DD=`defaults read com.apple.dt.Xcode IDECustomDerivedDataLocation 2>/dev/null`; \
	if [ $$CUSTOM_DD ]; then \
		echo clearing files in $$CUSTOM_DD older than one day; \
		find $$CUSTOM_DD/mapboxgl-app-* -mtime +1 | xargs rm -rf; \
		find $$CUSTOM_DD/osxapp-* -mtime +1 | xargs rm -rf; \
	fi; \
	if [ -d ~/Library/Developer/Xcode/DerivedData/ ] && [ ! $$CUSTOM_DD ]; then \
		echo 'clearing files in ~/Library/Developer/Xcode/DerivedData/{mapboxgl-app,osxapp}-* older than one day'; \
		find ~/Library/Developer/Xcode/DerivedData/mapboxgl-app-* -mtime +1 | xargs rm -rf; \
		find ~/Library/Developer/Xcode/DerivedData/osxapp-* -mtime +1 | xargs rm -rf; \
	fi
endif

.PHONY: clear_sqlite_cache
clear_sqlite_cache:
ifeq ($(BUILD), osx)
	rm -f ~/Library/Application\ Support/Mapbox\ GL/cache.db
else
	rm -f /tmp/mbgl-cache.db
endif

clean: clear_sqlite_cache clear_xcode_cache
	-find ./deps/gyp -name "*.pyc" -exec rm {} \;
	-rm -rf ./build/
	-rm -rf ./gyp/build/
	-rm -rf ./macosx/build
	-rm -rf ./linux/build
	-rm -rf ./ios/build
	-rm -rf ./test/build
	-rm -rf ./config/*.gypi
	-rm -rf ./platform/android/build \
	        ./platform/android/MapboxGLAndroidSDK/build \
	        ./platform/android/MapboxGLAndroidSDKTestApp/build \
	        ./platform/android/MapboxGLAndroidSDK/src/main/jniLibs \
	        ./platform/android/MapboxGLAndroidSDK/src/main/obj.target \
	        ./platform/android/MapboxGLAndroidSDK/src/main/assets

distclean: clean
	-rm -rf ./mason_packages
