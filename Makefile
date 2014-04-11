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

# Runs the test cases
test: config.gypi test/test.gyp
	deps/run_gyp test/test.gyp --depth=. -Goutput_dir=.. --generator-output=./build/test -f make
	make -C build/test V=$(V) test

run-tests: test
	@for FILE in build/Release/test_*; do \
		$${FILE}; \
	done


##### Makefile builds ##########################################################


# Builds the linux app with make. This is also used by Travis CI
linux: config.gypi linux/llmr-app.gyp
	deps/run_gyp linux/llmr-app.gyp --depth=. -Goutput_dir=.. --generator-output=./build/linux -f make
	make -C build/linux V=$(V) linuxapp

# Executes the linux binary
run-linux: linux
	build/Release/llmr



##### Maintenace operations ####################################################

clean:
	-rm -rf build/Release

distclean:
	-rm -rf build

.PHONY: llmr test linux
