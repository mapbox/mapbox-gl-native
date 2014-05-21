-include config.mk

BUILDTYPE ?= Release
PYTHON ?= python
V ?= 1

all: llmr

# Builds the regular library
llmr: config.gypi llmr.gyp node
	deps/run_gyp llmr.gyp --depth=. -Goutput_dir=.. --generator-output=./build/llmr -f make
	make -C build/llmr V=$(V) llmr-x86

node:
	@if [ ! `which node` ]; then echo 'error: depends on node.js. please make sure node is on your PATH'; exit 1; fi;

##### Test cases ###############################################################

build/test/Makefile: src common config.gypi test/test.gyp
	deps/run_gyp test/test.gyp --depth=. -Goutput_dir=.. --generator-output=./build/test -f make

test: build/test/Makefile
	make -C build/test BUILDTYPE=$(BUILDTYPE) V=$(V) test

# Runs all test cases
run-tests: test
	@for FILE in build/$(BUILDTYPE)/test_*; do \
		$${FILE}; \
	done

test/%:
	make -C build/test BUILDTYPE=$(BUILDTYPE) V=$(V) $*
	build/$(BUILDTYPE)/test_$*

# Only runs headless test case
run-headless-test: build/test/Makefile
	make -C build/test BUILDTYPE=Debug V=$(V) headless
	build/Debug/test_headless


##### Makefile builds ##########################################################


# Builds the linux app with make. This is also used by Travis CI
linux: config.gypi linux/llmr-app.gyp node
	deps/run_gyp linux/llmr-app.gyp --depth=. -Goutput_dir=.. --generator-output=./build/linux -f make
	make -C build/linux V=$(V) linuxapp

# Executes the Linux binary
run-linux: linux
	build/$(BUILDTYPE)/llmr



# Builds the OS X app with make.
osx: config.gypi macosx/llmr-app.gyp node
	deps/run_gyp macosx/llmr-app.gyp --depth=. -Goutput_dir=.. --generator-output=./build/macosx -f make
	make -C build/macosx V=$(V) osxapp

# Executes the OS X binary
run-osx: osx
	build/$(BUILDTYPE)/llmr.app/Contents/MacOS/llmr

##### Xcode projects ###########################################################

clear_xcode_cache:
    @CUSTOM_DD=`defaults read com.apple.dt.Xcode IDECustomDerivedDataLocation 2>/dev/null`; \
    if [[ $$CUSTOM_DD ]]; then \
        echo clearing files in $$CUSTOM_DD older than one day; \
        find $$CUSTOM_DD/llmr-app-* -mtime +1 | xargs rm -rf; \
    fi; \
    if [[ -d ~/Library/Developer/Xcode/DerivedData/ ]] && [[ ! $$CUSTOM_DD ]]; then \
        echo 'clearing files in ~/Library/Developer/Xcode/DerivedData/llmr-app-* older than one day'; \
        find ~/Library/Developer/Xcode/DerivedData/llmr-app-* -mtime +1 | xargs rm -rf; \
    fi

# build Mac OS X project for Xcode
xproj: config.gypi macosx/llmr-app.gyp clear_xcode_cache node
	deps/run_gyp macosx/llmr-app.gyp --depth=. --generator-output=./build -f xcode
	open ./build/macosx/llmr-app.xcodeproj

# build iOS project for Xcode
iproj: config.gypi ios/llmr-app.gyp clear_xcode_cache node
	deps/run_gyp ios/llmr-app.gyp --depth=. --generator-output=./build -f xcode
	open ./build/ios/llmr-app.xcodeproj

# build Linux project for Xcode (Runs on Mac OS X too, but without platform-specific code)
lproj: config.gypi linux/llmr-app.gyp clear_xcode_cache node
	deps/run_gyp linux/llmr-app.gyp --depth=. --generator-output=./build -f xcode
	open ./build/linux/llmr-app.xcodeproj


##### Maintenace operations ####################################################

clean: clear_xcode_cache
	-rm -rf ./build/Release
	-rm -rf ./build/Debug
	-rm -f include/llmr/shader/shaders.hpp
	-rm -f include/llmr/style/resources.hpp
	-rm -f src/style/resources.cpp

distclean: clean
	-rm -rf ./build

.PHONY: llmr test linux
