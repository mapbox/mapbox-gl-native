ifeq (,$(V))
	QUIET = @
endif

# Determine host platform
HOST ?= $(BUILD)

# Defines host defaults
include scripts/$(HOST)/defaults.mk

HOST_VERSION ?= $(BUILD_VERSION)

# Optionally include version-specific host defaults
-include scripts/$(HOST)/$(HOST_VERSION)/defaults.mk

export MASON_PLATFORM=$(HOST)
export MASON_PLATFORM_VERSION=$(HOST_VERSION)

HOST_SLUG = $(HOST)-$(HOST_VERSION)
CONFIGURE_FILES = scripts/$(HOST)/configure.sh
ifneq (,$(wildcard scripts/$(HOST)/$(HOST_VERSION)/configure.sh))
	CONFIGURE_FILES += scripts/$(HOST)/$(HOST_VERSION)/configure.sh
endif


# Text formatting
TEXT_BOLD = \033[1m
COLOR_GREEN = \033[32m
COLOR_CYAN = \033[36m
COLOR_PINK = \033[35m
FORMAT_END = \033[0m

default: ;

#### Dependencies ##############################################################

SUBMODULES += .mason/mason.sh
.mason/mason.sh:
	./scripts/flock.py .git/Submodule.lock git submodule update --init .mason

SUBMODULES += src/mbgl/util/geojsonvt/geojsonvt.hpp
src/mbgl/util/geojsonvt/geojsonvt.hpp:
	./scripts/flock.py .git/Submodule.lock git submodule update --init src/mbgl/util/geojsonvt

ifeq ($(HOST),ios)
SUBMODULES += platform/ios/vendor/SMCalloutView/SMCalloutView.h
platform/ios/vendor/SMCalloutView/SMCalloutView.h:
	./scripts/flock.py .git/Submodule.lock git submodule update --init platform/ios/vendor/SMCalloutView

SUBMODULES += test/ios/KIF/KIF.xcodeproj
test/ios/KIF/KIF.xcodeproj:
	./scripts/flock.py .git/Submodule.lock git submodule update --init test/ios/KIF
endif

# Wildcard targets get removed after build by default, but we want to preserve the config.
.PRECIOUS: config/%.gypi
config/%.gypi: $(SUBMODULES) configure $(CONFIGURE_FILES)
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Recreating project...$(FORMAT_END)\n"
	$(QUIET)$(ENV) ./scripts/flock.py build/Configure.lock ./configure config/$*.gypi


#### Build files ###############################################################

GYP_FLAGS += -Dhost=$(HOST)
GYP_FLAGS += -Iconfig/$(HOST_SLUG).gypi
GYP_FLAGS += -Dplatform_lib=$(PLATFORM)
GYP_FLAGS += -Dhttp_lib=$(HTTP)
GYP_FLAGS += -Dasset_lib=$(ASSET)
GYP_FLAGS += -Dcache_lib=$(CACHE)
GYP_FLAGS += -Dheadless_lib=$(HEADLESS)
GYP_FLAGS += -Dtest=$(BUILD_TEST)
GYP_FLAGS += -Drender=$(BUILD_RENDER)
GYP_FLAGS += --depth=.
GYP_FLAGS += -Goutput_dir=.
GYP_FLAGS += --generator-output=./build/$(HOST_SLUG)

.PHONY: Makefile/__project__
Makefile/__project__: print-env $(SUBMODULES) config/$(HOST_SLUG).gypi
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Recreating project...$(FORMAT_END)\n"
	$(QUIET)$(ENV) deps/run_gyp gyp/$(HOST).gyp $(GYP_FLAGS) \
		-f make$(GYP_FLAVOR_SUFFIX)

.PHONY: Xcode/__project__
Xcode/__project__: print-env $(SUBMODULES) config/$(HOST_SLUG).gypi
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Recreating project...$(FORMAT_END)\n"
	$(QUIET)$(ENV) deps/run_gyp gyp/$(HOST).gyp $(GYP_FLAGS) \
		-f xcode$(GYP_FLAVOR_SUFFIX)

#### Build individual targets ##################################################

NODE_PRE_GYP = $(shell npm bin)/node-pre-gyp
node/configure:
	$(QUIET)$(ENV) $(NODE_PRE_GYP) configure --clang -- \
	$(GYP_FLAGS) -Dlibuv_cflags= -Dlibuv_ldflags= -Dlibuv_static_libs=

node/xproj:
	$(QUIET)$(ENV) $(NODE_PRE_GYP) configure --clang -- \
	$(GYP_FLAGS) -f xcode -Dlibuv_cflags= -Dlibuv_ldflags= -Dlibuv_static_libs=
	$(QUIET)$(ENV) ./scripts/node/create_npm_scheme.sh test
	$(QUIET)$(ENV) ./scripts/node/create_npm_scheme.sh run test-suite

Makefile/node: Makefile/__project__ node/configure
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Building target node...$(FORMAT_END)\n"
	$(QUIET)$(ENV) $(NODE_PRE_GYP) build --clang -- \
 	-j$(JOBS)

Makefile/%: Makefile/__project__
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Building target $*...$(FORMAT_END)\n"
	$(QUIET)$(ENV) $(MAKE) -C build/$(HOST_SLUG) BUILDTYPE=$(BUILDTYPE) $*

Xcode/node: Xcode/__project__ node/xproj
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Building target node...$(FORMAT_END)\n"
	$(QUIET)$(ENV) set -o pipefail && xcodebuild \
		$(XCODEBUILD_ARGS) \
		-project ./build/binding.xcodeproj \
		-configuration $(BUILDTYPE) \
		-target mapbox-gl-native \
		-jobs $(JOBS) \
		$(XCPRETTY)

Xcode/%: Xcode/__project__
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Building target $*...$(FORMAT_END)\n"
	$(QUIET)$(ENV) set -o pipefail && xcodebuild \
		$(XCODEBUILD_ARGS) \
		-project ./build/$(HOST_SLUG)/gyp/$(HOST).xcodeproj \
		-configuration $(BUILDTYPE) \
		-target $* \
		-jobs $(JOBS) \
		$(XCPRETTY)

#### Run tests #################################################################

test-%: Makefile/test
	./scripts/run_tests.sh "build/$(HOST_SLUG)/$(BUILDTYPE)/test" --gtest_filter=$*

#### Helper targets ############################################################

.PHONY: print-env
print-env: $(SUBMODULES)
	@printf "$(COLOR_PINK)build:$(FORMAT_END) $(BUILD)/$(BUILD_VERSION)\n"
	@printf "$(COLOR_PINK)host:$(FORMAT_END) $(HOST)/$(HOST_VERSION)\n"
	@printf "$(COLOR_PINK)libs:$(FORMAT_END) "
		@printf "headless=$(COLOR_CYAN)%s$(FORMAT_END)  " $(HEADLESS)
		@printf "platform=$(COLOR_CYAN)%s$(FORMAT_END)  " $(PLATFORM)
		@printf "asset=$(COLOR_CYAN)%s$(FORMAT_END)  " $(ASSET)
		@printf "http=$(COLOR_CYAN)%s$(FORMAT_END)  " $(HTTP)
		@printf "cache=$(COLOR_CYAN)%s$(FORMAT_END)\n" $(CACHE)

# Never remove intermediate files
.SECONDARY:
