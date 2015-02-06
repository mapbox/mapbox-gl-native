BUILDTYPE ?= Release
MBGL ?= vendor/mbgl

DEBUG_FLAG =
ifeq ($(BUILDTYPE), Debug)
DEBUG_FLAG = -d
endif

ifeq ($(shell uname -s), Darwin)
HOST ?= osx
JOBS = $(shell sysctl -n hw.ncpu)
endif
ifeq ($(shell uname -s), Linux)
HOST ?= linux
JOBS = $(shell nproc)
endif

# Explicitly disable the default FileSource implementation
ASSET = none
HTTP = none
CACHE = none

include $(MBGL)/config/defaults.mk

global: build

.PHONY: build
build: build/Makefile
	@node-gyp build $(DEBUG_FLAG) -- -j$(JOBS)

.PHONY: vendor/mbgl
vendor/mbgl:
	git submodule update --init

.PHONY: build/Makefile
build/Makefile: $(MBGL)/config/$(HOST).gypi
	@node-gyp configure -- \
		-Dmbgl=$(MBGL) \
		-Dhost=$(HOST) \
		-I$(MBGL)/config/$(HOST).gypi \
		$(LIBS_$(HOST)) \
		-Duv_static_libs= -Duv_ldflags= \
		-Goutput_dir=.

$(MBGL)/config/%.gypi: $(MBGL) $(MBGL)/configure
	make -C $(MBGL) config/$*.gypi

.PHONY: test
test: build
	@`npm bin`/mocha -R spec

.PHONY: clean
clean:
	rm -rf build
	rm $(MBGL)/config.gypi
