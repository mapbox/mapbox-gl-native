-include config.mk

BUILDTYPE ?= Release
PYTHON ?= python
V ?= 1

all: llmr

# Builds the regular library
llmr: config.gypi llmr.gyp
	deps/run_gyp llmr.gyp --depth=. -Goutput_dir=.. --generator-output=./build/llmr -f make
	make -C build/llmr V=$(V) llmr-x86


##### Test cases ###############################################################

build/test/Makefile: src common config.gypi test/test.gyp
	deps/run_gyp test/test.gyp --depth=. -Goutput_dir=.. --generator-output=./build/test -f make

test: build/test/Makefile
	make -C build/test V=$(V) test

# Runs all test cases
run-tests: test
	@for FILE in build/Release/test_*; do \
		$${FILE}; \
	done

# Only runs headless test case
run-headless-test: build/test/Makefile
	make -C build/test BUILDTYPE=Debug V=$(V) headless
	build/Debug/test_headless


##### Makefile builds ##########################################################


# Builds the linux app with make. This is also used by Travis CI
linux: config.gypi linux/llmr-app.gyp
	deps/run_gyp linux/llmr-app.gyp --depth=. -Goutput_dir=.. --generator-output=./build/linux -f make
	make -C build/linux V=$(V) linuxapp

# Executes the Linux binary
run-linux: linux
	build/Release/llmr


##### Xcode projects ###########################################################

# build Mac OS X project for Xcode
xproj: config.gypi macosx/llmr-app.gyp
	deps/run_gyp macosx/llmr-app.gyp --depth=. --generator-output=./build -f xcode
	open ./build/macosx/llmr-app.xcodeproj

# build iOS project for Xcode
iproj: config.gypi ios/llmr-app.gyp
	deps/run_gyp ios/llmr-app.gyp --depth=. --generator-output=./build -f xcode
	open ./build/ios/llmr-app.xcodeproj

# build Linux project for Xcode (Runs on Mac OS X too, but without platform-specific code)
lproj: config.gypi linux/llmr-app.gyp
	deps/run_gyp linux/llmr-app.gyp --depth=. --generator-output=./build -f xcode
	open ./build/linux/llmr-app.xcodeproj


##### Maintenace operations ####################################################

clean:
	-rm -rf ./build/Release
	-rm -rf ./build/Debug

distclean: clean
	-rm -rf ./build

.PHONY: llmr test linux
