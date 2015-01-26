BUILDTYPE ?= Release
PYTHON ?= python
V ?= 1
PREFIX ?= /usr/local
JOBS ?= 1

ifeq ($(shell uname -s), Darwin)
PLATFORM ?= osx
endif
PLATFORM ?= linux

.PHONY: all
all: mbgl

config.gypi: configure
	./configure

config-ios.gypi: configure
	MASON_PLATFORM=ios ./configure config-ios.gypi

#### Library builds ############################################################

.PHONY: mbgl
mbgl: build/mbgl/Makefile
	$(MAKE) -C build/mbgl BUILDTYPE=$(BUILDTYPE) V=$(V) mbgl-core mbgl-$(PLATFORM) mbgl-headless

.PHONY: install
install: build/mbgl/Makefile
	$(MAKE) -C build/mbgl BUILDTYPE=$(BUILDTYPE) V=$(V) install

.PHONY: standalone
standalone: build/mbgl/Makefile
	LINK=`pwd`/gyp/link.py $(MAKE) -C build/mbgl BUILDTYPE=$(BUILDTYPE) V=$(V) standalone

#### Build scripts #############################################################

.PHONY: build/mbgl/Makefile
build/mbgl/Makefile: mbgl.gyp config.gypi
	deps/run_gyp mbgl.gyp -Iconfig.gypi -Dplatform=$(PLATFORM) --depth=. -Goutput_dir=.. --generator-output=./build/mbgl -f make

.PHONY: build/test/Makefile
build/test/Makefile: test/test.gyp config.gypi
	deps/run_gyp test/test.gyp -Iconfig.gypi -Dplatform=$(PLATFORM) --depth=. -Goutput_dir=.. --generator-output=./build/test -f make

.PHONY: build/linux/Makefile
build/linux/Makefile: linux/mapboxgl-app.gyp config.gypi
	deps/run_gyp linux/mapboxgl-app.gyp -Iconfig.gypi -Dplatform=linux --depth=. -Goutput_dir=.. --generator-output=./build/linux -f make

.PHONY: build/macosx/Makefile
build/macosx/Makefile: macosx/mapboxgl-app.gyp config.gypi
	deps/run_gyp macosx/mapboxgl-app.gyp -Iconfig.gypi -Dplatform=osx --depth=. -Goutput_dir=.. --generator-output=./build/macosx -f make

.PHONY: build/render/Makefile
build/render/Makefile: bin/render.gyp config.gypi
	deps/run_gyp bin/render.gyp -Iconfig.gypi -Dplatform=$(PLATFORM) --depth=. -Goutput_dir=.. --generator-output=./build/render -f make

.PHONY: build/test/test.xcodeproj
build/test/test.xcodeproj: test/test.gyp config.gypi
	deps/run_gyp test/test.gyp -Iconfig.gypi -Dplatform=$(PLATFORM) --depth=. -Goutput_dir=.. --generator-output=./build -f xcode

.PHONY: build/macosx/mapboxgl-app.xcodeproj
build/macosx/mapboxgl-app.xcodeproj: macosx/mapboxgl-app.gyp config.gypi
	deps/run_gyp macosx/mapboxgl-app.gyp -Iconfig.gypi -Dplatform=osx --depth=. --generator-output=./build -f xcode

.PHONY: build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj
build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj: ios/mapbox-gl-cocoa/app/mapboxgl-app.gyp config-ios.gypi
	deps/run_gyp ios/mapbox-gl-cocoa/app/mapboxgl-app.gyp -Iconfig-ios.gypi -Dplatform=ios --depth=. --generator-output=./build -f xcode

.PHONY: build/linux/mapboxgl-app.xcodeproj
build/linux/mapboxgl-app.xcodeproj: linux/mapboxgl-app.gyp config.gypi
	deps/run_gyp linux/mapboxgl-app.gyp -Iconfig.gypi -Dplatform=linux --depth=. --generator-output=./build -f xcode

.PHONY: build/bin/render.xcodeproj
	build/bin/render.xcodeproj: bin/render.gyp config.gypi
	deps/run_gyp bin/render.gyp -Iconfig.gypi -Dplatform=$(PLATFORM) --depth=. --generator-output=./build -f xcode

.PHONY: android
android:
	./scripts/local_mason.sh && \
	MASON_DIR=./.mason MASON_PLATFORM=android MASON_ANDROID_ABI=$(MASON_ANDROID_ABI) ./.mason/mason env PATH && \
	export CXX="`MASON_DIR=./.mason MASON_PLATFORM=android MASON_ANDROID_ABI=${MASON_ANDROID_ABI} ./.mason/mason env CXX`" && \
	export CC="`MASON_DIR=./.mason MASON_PLATFORM=android MASON_ANDROID_ABI=${MASON_ANDROID_ABI} ./.mason/mason env CC`" && \
	export LD="`MASON_DIR=./.mason MASON_PLATFORM=android MASON_ANDROID_ABI=${MASON_ANDROID_ABI} ./.mason/mason env LD`" && \
	export LINK="`MASON_DIR=./.mason MASON_PLATFORM=android MASON_ANDROID_ABI=${MASON_ANDROID_ABI} ./.mason/mason env CXX`" && \
	export AR="`MASON_DIR=./.mason MASON_PLATFORM=android MASON_ANDROID_ABI=${MASON_ANDROID_ABI} ./.mason/mason env AR`" && \
	export RANLIB="`MASON_DIR=./.mason MASON_PLATFORM=android MASON_ANDROID_ABI=${MASON_ANDROID_ABI} ./.mason/mason env RANLIB`" && \
	export STRIP="`MASON_DIR=./.mason MASON_PLATFORM=android MASON_ANDROID_ABI=${MASON_ANDROID_ABI} ./.mason/mason env STRIP`" && \
	export LDFLAGS="`MASON_DIR=./.mason MASON_PLATFORM=android MASON_ANDROID_ABI=${MASON_ANDROID_ABI} ./.mason/mason env LDFLAGS` ${LDFLAGS}" && \
	export CFLAGS="`MASON_DIR=./.mason MASON_PLATFORM= MASON_ANDROID_ABI=${MASON_ANDROID_ABI} ./.mason/mason env CFLAGS` ${CFLAGS}" && \
	export CPPFLAGS="`MASON_DIR=./.mason MASON_PLATFORM=android MASON_ANDROID_ABI=${MASON_ANDROID_ABI} ./.mason/mason env CPPFLAGS` ${CPPFLAGS}" && \
	export PATH="`MASON_DIR=./.mason MASON_PLATFORM=android MASON_ANDROID_ABI=${MASON_ANDROID_ABI} ./.mason/mason env PATH`:${PATH}" && \
	export JNIDIR="`MASON_DIR=./.mason MASON_PLATFORM=android MASON_ANDROID_ABI=${MASON_ANDROID_ABI} ./.mason/mason env JNIDIR`" && \
	MASON_PLATFORM=android MASON_ANDROID_ABI=$(MASON_ANDROID_ABI) ./configure config-android.gypi && \
	deps/run_gyp android/mapboxgl-app.gyp -Iconfig-android.gypi -Dplatform=android --depth=. --generator-output=./build/android/$(MASON_ANDROID_ABI) -f make-android && \
	$(MAKE) -C build/android/$(MASON_ANDROID_ABI) -j$(JOBS) BUILDTYPE=$(BUILDTYPE) V=$(V) androidapp && \
	BUILDTYPE=$(BUILDTYPE) MASON_ANDROID_ABI=$(MASON_ANDROID_ABI) ./android/scripts/copy-files.sh && \
	cd android/java && \
	./gradlew --parallel-threads=$(JOBS) build

##### Test cases ###############################################################

test: build/test/Makefile
	$(MAKE) -C build/test BUILDTYPE=$(BUILDTYPE) V=$(V) test

test-%: test
	./scripts/run_tests.sh --gtest_filter=$*

# build Mac OS X project for Xcode
xtest: build/test/test.xcodeproj
	open ./build/test/test.xcodeproj

##### Makefile builds ##########################################################


# Builds the linux app with make.
linux: build/linux/Makefile
	$(MAKE) -C build/linux BUILDTYPE=$(BUILDTYPE) V=$(V) linuxapp

# Executes the Linux binary
run-linux: linux
	(cd build/$(BUILDTYPE) && ./mapbox-gl)

# Builds the OS X app with make.
osx: build/macosx/Makefile
	$(MAKE) -C build/macosx BUILDTYPE=$(BUILDTYPE) V=$(V) osxapp

# Executes the OS X binary
run-osx: osx
	build/$(BUILDTYPE)/Mapbox\ GL.app/Contents/MacOS/MAPBOX\ GL

# Builds the CLI render app
render: build/render/Makefile
	$(MAKE) -C build/render BUILDTYPE=$(BUILDTYPE) V=$(V) mbgl-render

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
