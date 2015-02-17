BUILDTYPE ?= Release
MBGL ?= vendor/mbgl

DEBUG_FLAG =
ifeq ($(BUILDTYPE), Debug)
DEBUG_FLAG = -d
endif

ifeq ($(shell uname -s), Darwin)
HOST ?= osx
ifeq ($(JOBS),)
JOBS = $(shell sysctl -n hw.ncpu)
endif
endif
ifeq ($(shell uname -s), Linux)
HOST ?= linux
ifeq ($(JOBS),)
JOBS = $(shell nproc)
endif
endif

# Explicitly disable the default FileSource implementation
ASSET = none
HTTP = none
CACHE = none

include $(MBGL)/config/defaults.mk

global: build

.PHONY: build
build: build/Makefile
	@node-pre-gyp build $(DEBUG_FLAG) --clang -- -j$(JOBS)

vendor/mbgl:
	git submodule update --init

.PHONY: build/Makefile
build/Makefile: $(MBGL)/config/$(HOST).gypi
	@node-pre-gyp configure --clang -- \
		-Dmbgl=$(MBGL) \
		-Dhost=$(HOST) \
		-I$(MBGL)/config/$(HOST).gypi \
		$(LIBS_$(HOST)) \
		-Duv_static_libs= -Duv_ldflags= \
		-Goutput_dir=.

$(MBGL)/config/%.gypi: $(MBGL) $(MBGL)/configure
	make -C $(MBGL) config/$*.gypi

.PHONY: test-suite
test-suite: build
	-@`npm bin`/tape test/render.test.js

.PHONY: test-js
test-js: build
	@`npm bin`/tape test/js/**/*.test.js

.PHONY: test
test: test-js test-suite

.PHONY: clean
clean:
	rm -rf build lib
	rm -f $(MBGL)/config/$(HOST).gypi
