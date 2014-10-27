BUILDTYPE ?= Release
PYTHON ?= python
V ?= 1
PREFIX ?= /usr/local

ifeq ($(shell uname -s), Darwin)
PLATFORM ?= osx
endif
PLATFORM ?= linux


all: mbgl

config.gypi: configure
	./configure

config-ios.gypi: configure
	MASON_PLATFORM=ios ./configure config-ios.gypi

# Builds the regular library
mbgl: config.gypi mapboxgl.gyp
	deps/run_gyp mapboxgl.gyp -Iconfig.gypi -Dplatform=$(PLATFORM) --depth=. -Goutput_dir=.. --generator-output=./build/mbgl -f make
	$(MAKE) -C build/mbgl BUILDTYPE=$(BUILDTYPE) V=$(V) mbgl

install: config.gypi mapboxgl.gyp
	deps/run_gyp mapboxgl.gyp -Iconfig.gypi -Dplatform=$(PLATFORM) -Dinstall_prefix=$(PREFIX) --depth=. -Goutput_dir=.. --generator-output=./build/mbgl -f make
	$(MAKE) -C build/mbgl BUILDTYPE=$(BUILDTYPE) V=$(V) install

##### Test cases ###############################################################

build/test/Makefile: src common config.gypi test/test.gyp
	deps/run_gyp test/test.gyp -Iconfig.gypi -Dplatform=$(PLATFORM) --depth=. -Goutput_dir=.. --generator-output=./build/test -f make

test: build/test/Makefile
	$(MAKE) -C build/test BUILDTYPE=$(BUILDTYPE) V=$(V) test

test_%: build/test/Makefile
	$(MAKE) -C build/test BUILDTYPE=$(BUILDTYPE) V=$(V) $*
	(cd build/$(BUILDTYPE) && exec ./test_$*)

# build Mac OS X project for Xcode
xtest: config.gypi clear_xcode_cache
	deps/run_gyp test/test.gyp -Iconfig.gypi -Dplatform=osx --depth=. -Goutput_dir=.. --generator-output=./build -f xcode
	open ./build/test/test.xcodeproj

##### Makefile builds ##########################################################


# Builds the linux app with make.
linux: config.gypi linux/mapboxgl-app.gyp
	deps/run_gyp linux/mapboxgl-app.gyp -Iconfig.gypi -Dplatform=linux --depth=. -Goutput_dir=.. --generator-output=./build/linux -f make
	$(MAKE) -C build/linux BUILDTYPE=$(BUILDTYPE) V=$(V) linuxapp

# Executes the Linux binary
run-linux: linux
	(cd build/$(BUILDTYPE) && ./mapbox-gl)



# Builds the OS X app with make.
osx: config.gypi macosx/mapboxgl-app.gyp
	deps/run_gyp macosx/mapboxgl-app.gyp -Iconfig.gypi -Dplatform=osx --depth=. -Goutput_dir=.. --generator-output=./build/macosx -f make
	$(MAKE) -C build/macosx BUILDTYPE=$(BUILDTYPE) V=$(V) osxapp

# Executes the OS X binary
run-osx: osx
	build/$(BUILDTYPE)/Mapbox\ GL.app/Contents/MacOS/MAPBOX\ GL

##### Xcode projects ###########################################################

clear_xcode_cache:
    @CUSTOM_DD=`defaults read com.apple.dt.Xcode IDECustomDerivedDataLocation 2>/dev/null`; \
    if [[ $$CUSTOM_DD ]]; then \
        echo clearing files in $$CUSTOM_DD older than one day; \
        find $$CUSTOM_DD/mapboxgl-app-* -mtime +1 | xargs rm -rf; \
    fi; \
    if [[ -d ~/Library/Developer/Xcode/DerivedData/ ]] && [[ ! $$CUSTOM_DD ]]; then \
        echo 'clearing files in ~/Library/Developer/Xcode/DerivedData/mapboxgl-app-* older than one day'; \
        find ~/Library/Developer/Xcode/DerivedData/mapboxgl-app-* -mtime +1 | xargs rm -rf; \
    fi

# build Mac OS X project for Xcode
xproj-cli: config.gypi macosx/mapboxgl-app.gyp clear_xcode_cache
	deps/run_gyp macosx/mapboxgl-app.gyp -Iconfig.gypi -Dplatform=osx --depth=. --generator-output=./build -f xcode

xproj: xproj-cli
	open ./build/macosx/mapboxgl-app.xcodeproj

# build iOS project for Xcode
iproj-cli: config-ios.gypi ios/mapbox-gl-cocoa/app/mapboxgl-app.gyp clear_xcode_cache
	deps/run_gyp ios/mapbox-gl-cocoa/app/mapboxgl-app.gyp -Iconfig-ios.gypi -Dplatform=ios --depth=. --generator-output=./build -f xcode

iproj: iproj-cli
	open ./build/ios/mapbox-gl-cocoa/app/mapboxgl-app.xcodeproj

# build Linux project for Xcode (Runs on Mac OS X too, but without platform-specific code)
lproj: config.gypi linux/mapboxgl-app.gyp clear_xcode_cache
	deps/run_gyp linux/mapboxgl-app.gyp -Iconfig.gypi -Dplatform=linux --depth=. --generator-output=./build -f xcode
	open ./build/linux/mapboxgl-app.xcodeproj


##### Maintenace operations ####################################################

clean: clear_xcode_cache
	-find ./deps/gyp -name "*.pyc" -exec rm {} \;
	-rm -rf ./build/
	-rm -rf ./config.gypi ./config-ios.gypi

distclean: clean
	-rm -rf ./mason_packages

.PHONY: mbgl install test linux clear_xcode_cache build/test/Makefile clean distclean
# DO NOT DELETE
