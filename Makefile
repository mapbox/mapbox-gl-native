export BUILDTYPE ?= Release
export BUILD_TEST ?= 1
export BUILD_RENDER ?= 1
export BUILD_OFFLINE ?= 1
export ENABLE_COVERAGE ?= 0

# Determine build platform
ifeq ($(shell uname -s), Darwin)
export BUILD = osx
export JOBS ?= $(shell sysctl -n hw.ncpu)
else ifeq ($(shell uname -s), Linux)
export BUILD = linux
export JOBS ?= $(shell grep --count processor /proc/cpuinfo)
else
$(error Cannot determine build platform)
endif

export BUILD_VERSION = $(shell uname -m)

RUN =  +$(MAKE) -f scripts/main.mk

default:
	@printf "You must specify a valid target\n"

#### OS X targets ##############################################################

.PHONY: osx
osx:
	$(RUN) HOST=osx HOST_VERSION=x86_64 Xcode/osxapp

.PHONY: run-osx
run-osx: osx
	"gyp/build/$(BUILDTYPE)/Mapbox GL.app/Contents/MacOS/Mapbox GL"

.PHONY: Xcode/osx
Xcode/osx:
	$(RUN) HOST=osx HOST_VERSION=x86_64 Xcode/__project__

.PHONY: xproj
xproj: Xcode/osx
	open ./build/osx-x86_64/gyp/osx.xcodeproj

.PHONY: xpackage
xpackage: Xcode/osx
	./platform/osx/scripts/package.sh

.PHONY: xpackage-strip
xpackage-strip: Xcode/osx
	./platform/osx/scripts/package.sh strip

.PHONY: xtest
xtest:
	$(RUN) HOST=osx HOST_VERSION=x86_64 Xcode/test

.PHONY: xctest
xctest: Xcode/osx
	./platform/osx/scripts/test.sh

#### iOS targets ##############################################################

.PHONY: Xcode/ios
Xcode/ios:
	$(RUN) HOST=ios Xcode/__project__

.PHONY: iproj
iproj: Xcode/ios
	open ./build/ios-all/gyp/ios.xcodeproj

.PHONY: ios
ios:
	$(RUN) HOST=ios XCODEBUILD_ARGS='-sdk iphoneos ARCHS="arm64 armv7 armv7s"' Xcode/iosapp

.PHONY: isim
isim:
	$(RUN) HOST=ios XCODEBUILD_ARGS='-sdk iphonesimulator ARCHS="x86_64 i386"' Xcode/iosapp

.PHONY: ibench
ibench:
	$(RUN) HOST=ios XCODEBUILD_ARGS='-sdk iphoneos ARCHS="arm64"' Xcode/ios-bench

.PHONY: ipackage
ipackage: Xcode/ios
	BITCODE=$(BITCODE) FORMAT=$(FORMAT) BUILD_DEVICE=$(BUILD_DEVICE) SYMBOLS=$(SYMBOLS) \
	BUNDLE_RESOURCES=YES PLACE_RESOURCE_BUNDLES_OUTSIDE_FRAMEWORK=YES \
	./platform/ios/scripts/package.sh

.PHONY: ipackage-strip
ipackage-strip: Xcode/ios
	BITCODE=$(BITCODE) FORMAT=$(FORMAT) BUILD_DEVICE=$(BUILD_DEVICE) SYMBOLS=NO \
	BUNDLE_RESOURCES=YES PLACE_RESOURCE_BUNDLES_OUTSIDE_FRAMEWORK=YES \
	./platform/ios/scripts/package.sh

.PHONY: ipackage-sim
ipackage-sim: Xcode/ios
	BUILDTYPE=Debug BITCODE=$(BITCODE) FORMAT=dynamic BUILD_DEVICE=false SYMBOLS=$(SYMBOLS) \
	BUNDLE_RESOURCES=YES PLACE_RESOURCE_BUNDLES_OUTSIDE_FRAMEWORK=YES \
	./platform/ios/scripts/package.sh

.PHONY: iframework
iframework: Xcode/ios
	BITCODE=$(BITCODE) FORMAT=dynamic BUILD_DEVICE=$(BUILD_DEVICE) SYMBOLS=$(SYMBOLS) \
	./platform/ios/scripts/package.sh

.PHONY: ifabric
ifabric: Xcode/ios
	BITCODE=$(BITCODE) FORMAT=$(FORMAT) BUILD_DEVICE=$(BUILD_DEVICE) SYMBOLS=NO BUNDLE_RESOURCES=YES \
	./platform/ios/scripts/package.sh

.PHONY: itest
itest: ipackage-sim
	./platform/ios/scripts/test.sh

.PHONY: idocument
idocument:
	OUTPUT=$(OUTPUT) ./platform/ios/scripts/document.sh

#### Linux targets #####################################################

.PHONY: linux
linux:
	$(RUN) Makefile/linuxapp

.PHONY: nlinux
nlinux:
	$(RUN) Ninja/linuxapp

.PHONY: run-linux
run-linux: linux
	(cd build/$(BUILD)-$(BUILD_VERSION)/$(BUILDTYPE) && ./mapbox-gl)

.PHONY: run-valgrind-linux
run-valgrind-linux: linux
	(cd build/$(BUILD)-$(BUILD_VERSION)/$(BUILDTYPE) && valgrind --leak-check=full --suppressions=../../../scripts/valgrind.sup ./mapbox-gl)

#### Android targets #####################################################

# Builds a particular android architecture.
android-lib-%:
	$(RUN) HOST=android HOST_VERSION=$* Makefile/androidapp

# Builds the default Android library
.PHONY: android-lib
android-lib:
	$(RUN) HOST=android Makefile/androidapp

# Builds the selected/default Android library
.PHONY: android
android: android-lib
	cd platform/android && ./gradlew --parallel --max-workers=$(JOBS) assemble$(BUILDTYPE)

# Builds all android architectures for distribution.
.PHONY: apackage
apackage: android-lib-arm-v5 android-lib-arm-v7 android-lib-arm-v8 android-lib-x86 android-lib-x86-64 android-lib-mips
	cd platform/android && ./gradlew --parallel-threads=$(JOBS) assemble$(BUILDTYPE)

#### Node targets #####################################################

# Builds the Node.js library
.PHONY: node
node:
	$(RUN) LOOP=uv HTTP=none ASSET=none Makefile/node

.PHONY: Xcode/node
Xcode/node:
	$(RUN) LOOP=uv HTTP=none ASSET=none Xcode/node

.PHONY: xnode
xnode: Xcode/node
	open ./build/binding.xcodeproj

.PHONY: nproj
nproj:
	$(RUN) HTTP=none ASSET=none node/xproj
	open ./build/binding.xcodeproj

#### Miscellaneous targets #####################################################

.PHONY: test
test:
	$(RUN) Makefile/test

test-%:
	$(RUN) test-$*

.PHONY: check
check:
	$(RUN) BUILDTYPE=Debug ENABLE_COVERAGE=1 check

coveralls:
	$(RUN) BUILDTYPE=Debug ENABLE_COVERAGE=1 coveralls

.PHONY: render
render:
	$(RUN) Makefile/mbgl-render

.PHONY: offline
offline:
	$(RUN) Makefile/mbgl-offline

.PHONY: config
config:
	$(RUN) config

# Generates a compilation database with ninja for use in clang tooling
.PHONY: compdb
compdb:
	$(RUN) Ninja/compdb

.PHONY: tidy
tidy:
	$(RUN) tidy

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

clean: clear_xcode_cache
	-find ./deps/gyp -name "*.pyc" -exec rm {} \;
	-find ./build -type f -not -path '*/*.xcodeproj/*' -exec rm {} \;
	-rm -rf ./gyp/build/
	-rm -rf ./config/*.gypi
	-rm -rf ./platform/android/MapboxGLAndroidSDK/build \
	        ./platform/android/MapboxGLAndroidSDKTestApp/build \
	        ./platform/android/MapboxGLAndroidSDK/src/main/jniLibs \
	        ./platform/android/MapboxGLAndroidSDKTestApp/src/main/jniLibs \
	        ./platform/android/MapboxGLAndroidSDK/src/main/obj.target \
	        ./platform/android/MapboxGLAndroidSDK/src/main/assets

distclean: clean
	-rm -rf ./mason_packages
