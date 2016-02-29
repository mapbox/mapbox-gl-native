ifeq (,$(V))
	QUIET = @
endif

# Determine host platform
export HOST ?= $(BUILD)

# Defines host defaults
include platform/$(HOST)/scripts/defaults.mk

export HOST_VERSION ?= $(BUILD_VERSION)

# Optionally include version-specific host defaults
-include scripts/$(HOST)/$(HOST_VERSION)/defaults.mk
-include platform/$(HOST)/scripts/$(HOST_VERSION)/defaults.mk

export MASON_PLATFORM=$(HOST)
export MASON_PLATFORM_VERSION=$(HOST_VERSION)

export HOST_SLUG = $(HOST)-$(HOST_VERSION)
CONFIGURE_FILES = platform/$(HOST)/scripts/configure.sh
ifneq (,$(wildcard scripts/$(HOST)/$(HOST_VERSION)/configure.sh))
	CONFIGURE_FILES += scripts/$(HOST)/$(HOST_VERSION)/configure.sh
endif
ifneq (,$(wildcard platform/$(HOST)/scripts/$(HOST_VERSION)/configure.sh))
	CONFIGURE_FILES += platform/$(HOST)/scripts/$(HOST_VERSION)/configure.sh
endif

ifneq (,$(findstring clang,$(CXX)))
	CXX_HOST = "clang"
else ifneq (,$(findstring g++,$(CXX)))
	CXX_HOST = "g++"
endif

# Text formatting
TEXT_BOLD = \033[1m
COLOR_GREEN = \033[32m
COLOR_CYAN = \033[36m
COLOR_PINK = \033[35m
FORMAT_END = \033[0m

default: ;

#### Dependencies ##############################################################

ifneq (,$(wildcard .git/.))
SUBMODULES += .mason/mason.sh
.mason/mason.sh:
	./scripts/flock.py .git/Submodule.lock git submodule update --init .mason

ifeq ($(HOST),ios)
SUBMODULES += platform/ios/vendor/SMCalloutView/SMCalloutView.h
platform/ios/vendor/SMCalloutView/SMCalloutView.h:
	./scripts/flock.py .git/Submodule.lock git submodule update --init platform/ios/vendor/SMCalloutView

SUBMODULES += platform/ios/test/KIF/KIF.xcodeproj
platform/ios/test/KIF/KIF.xcodeproj:
	./scripts/flock.py .git/Submodule.lock git submodule update --init platform/ios/test/KIF
endif
endif

# Wildcard targets get removed after build by default, but we want to preserve the config.
.PRECIOUS: config/%.gypi
config/%.gypi: $(SUBMODULES) configure $(CONFIGURE_FILES)
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Recreating project...$(FORMAT_END)\n"
	$(QUIET)$(ENV) ./scripts/flock.py build/Configure.lock ./configure config/$*.gypi

.PHONY: config
config: config/$(HOST_SLUG).gypi

#### Build files ###############################################################

GYP_FLAGS += -Dhost=$(HOST)
GYP_FLAGS += -Iconfig/$(HOST_SLUG).gypi
GYP_FLAGS += -Dplatform_lib=$(PLATFORM)
GYP_FLAGS += -Dhttp_lib=$(HTTP)
GYP_FLAGS += -Dloop_lib=$(LOOP)
GYP_FLAGS += -Dasset_lib=$(ASSET)
GYP_FLAGS += -Dheadless_lib=$(HEADLESS)
GYP_FLAGS += -Dtest=$(BUILD_TEST)
GYP_FLAGS += -Drender=$(BUILD_RENDER)
GYP_FLAGS += -Doffline=$(BUILD_OFFLINE)
GYP_FLAGS += -Dcoverage=$(ENABLE_COVERAGE)
GYP_FLAGS += -Dcxx_host=$(CXX_HOST)
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

.PHONY: Ninja/__project__
Ninja/__project__: print-env $(SUBMODULES) config/$(HOST_SLUG).gypi
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Recreating project...$(FORMAT_END)\n"
	$(QUIET)$(ENV) deps/run_gyp gyp/$(HOST).gyp -Gconfig=$(BUILDTYPE) $(GYP_FLAGS) \
		-f ninja

#### Build individual targets ##################################################

NODE_PRE_GYP = $(shell npm bin)/node-pre-gyp
node/configure:
	$(QUIET)$(ENV) $(NODE_PRE_GYP) configure --clang -- \
	$(GYP_FLAGS) -Dlibuv_cflags= -Dlibuv_ldflags= -Dlibuv_static_libs=

node/xproj: Xcode/__project__ node/configure
	$(QUIET)$(ENV) $(NODE_PRE_GYP) configure --clang -- \
	$(GYP_FLAGS) -f xcode -Dlibuv_cflags= -Dlibuv_ldflags= -Dlibuv_static_libs=
	$(QUIET)$(ENV) ./platform/node/scripts/create_node_scheme.sh "node test" "`npm bin tape`/tape platform/node/test/js/**/*.test.js"
	$(QUIET)$(ENV) ./platform/node/scripts/create_node_scheme.sh "npm run test-suite" "platform/node/test/render.test.js"

Makefile/node: Makefile/__project__ node/configure
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Building target node...$(FORMAT_END)\n"
	$(QUIET)$(ENV) $(NODE_PRE_GYP) build --clang -- \
 	-j$(JOBS)

Makefile/%: Makefile/__project__
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Building target $*...$(FORMAT_END)\n"
	$(QUIET)$(ENV) $(MAKE) -C build/$(HOST_SLUG) BUILDTYPE=$(BUILDTYPE) $*

Xcode/node: Xcode/__project__ node/xproj
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Building target node...$(FORMAT_END)\n"
	xcodebuild \
		$(XCODEBUILD_ARGS) \
		-project ./build/binding.xcodeproj \
		-configuration $(BUILDTYPE) \
		-target mapbox-gl-native \
		-jobs $(JOBS)

Xcode/%: Xcode/__project__
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Building target $*...$(FORMAT_END)\n"
	xcodebuild \
		PROVISIONING_PROFILE="$(PROVISIONING_PROFILE)" \
		$(XCODEBUILD_ARGS) \
		-project ./build/$(HOST_SLUG)/gyp/$(HOST).xcodeproj \
		-configuration $(BUILDTYPE) \
		-target $* \
		-jobs $(JOBS)

Ninja/%: Ninja/__project__
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Building target $*...$(FORMAT_END)\n"
	$(QUIET)$(ENV) deps/ninja/ninja-$(BUILD) -C build/$(HOST_SLUG)/$(BUILDTYPE) $*


Ninja/compdb: OUTPUT=build/$(HOST_SLUG)/$(BUILDTYPE)/compile_commands.json
Ninja/compdb: Ninja/__project__
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Writing to $(OUTPUT)$(FORMAT_END)\n"
	$(QUIET)$(ENV) deps/ninja/ninja-$(BUILD) -C build/$(HOST_SLUG)/$(BUILDTYPE) \
		-t compdb cc cc_s cxx objc objcxx > $(OUTPUT)

#### Tidy ######################################################################

tidy: Ninja/compdb
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Generating header files...$(FORMAT_END)\n"
	$(QUIET)$(ENV) deps/ninja/ninja-$(BUILD) -C build/$(HOST_SLUG)/$(BUILDTYPE) version shaders
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Running tidy...$(FORMAT_END)\n"
	@./scripts/clang-tidy.sh

#### Run tests #################################################################

test-%: Makefile/test
	./scripts/run_tests.sh "build/$(HOST_SLUG)/$(BUILDTYPE)/test" --gtest_filter=$*

check: Makefile/test
	./scripts/collect-coverage.sh

coveralls: Makefile/test
	./scripts/coveralls.sh

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
		@printf "loop=$(COLOR_CYAN)%s$(FORMAT_END)  " $(LOOP)
		@printf "\n"

# Never remove intermediate files
.SECONDARY:
