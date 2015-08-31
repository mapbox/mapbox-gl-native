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

NODE_PRE_GYP = $(shell which node-pre-gyp)
ifeq ($(NODE_PRE_GYP),)
NODE_PRE_GYP = $(shell npm bin)/node-pre-gyp
endif

# Explicitly disable the default FileSource implementation
ASSET = none
HTTP = none
CACHE = none

include $(MBGL)/config/defaults.mk

global: build

.PHONY: build
build: build/Makefile
	@$(NODE_PRE_GYP) build $(DEBUG_FLAG) --clang -- -j$(JOBS)

vendor/mbgl:
	git submodule update --init

GYP_SETTINGS = -Dmbgl=$(MBGL) \
	-Dhost=$(HOST) \
	-I$(MBGL)/config/$(HOST).gypi \
	$(LIBS_$(HOST)) \
	-Duv_static_libs= -Duv_ldflags=

.PHONY: build/Makefile
build/Makefile: $(MBGL)/config/$(HOST).gypi
	@$(NODE_PRE_GYP) configure --clang -- $(GYP_SETTINGS) -f make

.PHONY: build/binding.xcodeproj
build/binding.xcodeproj: $(MBGL)/config/$(HOST).gypi
	@$(NODE_PRE_GYP) configure --clang -- $(GYP_SETTINGS) -f xcode

.PHONY: xproj
xproj: build/binding.xcodeproj
	open build/binding.xcodeproj

$(MBGL)/config/%.gypi: $(MBGL) $(MBGL)/configure
	make -C $(MBGL) config/$*.gypi geojsonvt

.PHONY: test-suite
test-suite:
	-@(`npm bin`/tape test/render.test.js)

.PHONY: test-js
test-js:
	@(`npm bin`/tape test/js/**/*.test.js)

.PHONY: test
test: test-js test-suite

.PHONY: clean
clean:
	rm -rf build lib
	rm -f $(MBGL)/config/$(HOST).gypi
