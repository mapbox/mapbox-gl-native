ifeq ($(shell uname -s), Darwin)
  export PLATFORM ?= osx
else ifeq ($(shell uname -s), Linux)
  export PLATFORM ?= linux
endif

ifeq ($(PLATFORM),android)
  export SUBPLATFORM ?= arm-v7
  GYP_FLAVOR_SUFFIX = -android
  ENV = $(shell MASON_ANDROID_ABI=$(SUBPLATFORM) ./platform/android/scripts/toolchain.sh)
else ifeq ($(PLATFORM),ios)
  export SUBPLATFORM = all
else ifeq ($(PLATFORM),osx)
  export SUBPLATFORM = x86_64
else
  export SUBPLATFORM ?= $(shell uname -m)
endif

ifneq ($(PLATFORM),node)
  export MASON_PLATFORM = $(PLATFORM)
  export MASON_PLATFORM_VERSION = $(SUBPLATFORM)
  export PLATFORM_SLUG = $(PLATFORM)-$(SUBPLATFORM)
else ifeq ($(shell uname -s), Darwin)
  export MASON_PLATFORM = osx
  export MASON_PLATFORM_VERSION = $(SUBPLATFORM)
  export PLATFORM_SLUG = node-osx-$(SUBPLATFORM)
else ifeq ($(shell uname -s), Linux)
  export MASON_PLATFORM = linux
  export MASON_PLATFORM_VERSION = $(SUBPLATFORM)
  export PLATFORM_SLUG = node-linux-$(SUBPLATFORM)
endif

export PLATFORM_OUTPUT = ./build/$(PLATFORM_SLUG)
export PLATFORM_CONFIG_INPUT = platform/$(MASON_PLATFORM)/scripts/configure.sh
export PLATFORM_CONFIG_OUTPUT = $(PLATFORM_OUTPUT)/config.gypi

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

# Never remove intermediate files
.SECONDARY:

#### Dependencies ##############################################################

ifneq (,$(wildcard .git/.))
.mason:
	git submodule update --init
else
.mason: ;
endif

$(PLATFORM_CONFIG_OUTPUT): .mason configure $(PLATFORM_CONFIG_INPUT)
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Running configure...$(FORMAT_END)\n"
	$(ENV) ./configure $(PLATFORM_CONFIG_INPUT) $(PLATFORM_CONFIG_OUTPUT)

#### Build files ###############################################################

GYP_FLAGS += -I$(PLATFORM_CONFIG_OUTPUT)
GYP_FLAGS += -Dcoverage=$(ENABLE_COVERAGE)
GYP_FLAGS += -Dcxx_host=$(CXX_HOST)
GYP_FLAGS += -Goutput_dir=.
GYP_FLAGS += --depth=.
GYP_FLAGS += --generator-output=$(PLATFORM_OUTPUT)

.PHONY: Makefile/__project__
Makefile/__project__: $(PLATFORM_CONFIG_OUTPUT)
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Recreating project...$(FORMAT_END)\n"
	$(ENV) deps/run_gyp platform/$(PLATFORM)/platform.gyp $(GYP_FLAGS) -f make$(GYP_FLAVOR_SUFFIX)

.PHONY: Xcode/__project__
Xcode/__project__: $(PLATFORM_CONFIG_OUTPUT)
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Recreating project...$(FORMAT_END)\n"
	$(ENV) deps/run_gyp platform/$(PLATFORM)/platform.gyp $(GYP_FLAGS) -f xcode$(GYP_FLAVOR_SUFFIX)

.PHONY: Ninja/__project__
Ninja/__project__: $(PLATFORM_CONFIG_OUTPUT)
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Recreating project...$(FORMAT_END)\n"
	$(ENV) deps/run_gyp platform/$(PLATFORM)/platform.gyp $(GYP_FLAGS) -f ninja

#### Node targets ##############################################################

NODE_PRE_GYP = $(shell npm bin)/node-pre-gyp

Makefile/node: $(PLATFORM_CONFIG_OUTPUT)
	$(NODE_PRE_GYP) configure --clang -- $(GYP_FLAGS) \
	  -Dlibuv_cflags= -Dlibuv_ldflags= -Dlibuv_static_libs=
	$(NODE_PRE_GYP) build --clang

Xcode/node: $(PLATFORM_CONFIG_OUTPUT)
	$(NODE_PRE_GYP) configure --clang -- $(GYP_FLAGS) -f xcode \
	  -Dlibuv_cflags= -Dlibuv_ldflags= -Dlibuv_static_libs=

#### Build individual targets ##################################################

Makefile/%: Makefile/__project__
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Building target $*...$(FORMAT_END)\n"
	$(ENV) $(MAKE) -j$(JOBS) -C $(PLATFORM_OUTPUT) BUILDTYPE=$(BUILDTYPE) $*

Xcode/%: Xcode/__project__
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Building target $*...$(FORMAT_END)\n"
	set -o pipefail && xcodebuild \
		CODE_SIGNING_REQUIRED=NO \
		CODE_SIGN_IDENTITY= \
		-project $(PLATFORM_OUTPUT)/platform/$(PLATFORM)/platform.xcodeproj \
		-configuration $(BUILDTYPE) \
		-target $* \
		-jobs $(JOBS) | xcpretty

Ninja/%: Ninja/__project__
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Building target $*...$(FORMAT_END)\n"
	$(ENV) deps/ninja/ninja-$(PLATFORM) -C $(PLATFORM_OUTPUT)/$(BUILDTYPE) $*

#### Tidy ######################################################################

Ninja/compdb: OUTPUT=$(PLATFORM_OUTPUT)/$(BUILDTYPE)/compile_commands.json
Ninja/compdb: Ninja/__project__
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Writing to $(OUTPUT)$(FORMAT_END)\n"
	$(ENV) deps/ninja/ninja-$(PLATFORM) -C $(PLATFORM_OUTPUT)/$(BUILDTYPE) \
		-t compdb cc cc_s cxx objc objcxx > $(OUTPUT)

tidy: Ninja/compdb
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Generating header files...$(FORMAT_END)\n"
	$(ENV) deps/ninja/ninja-$(PLATFORM) -C $(PLATFORM_OUTPUT)/$(BUILDTYPE) version shaders
	@printf "$(TEXT_BOLD)$(COLOR_GREEN)* Running tidy...$(FORMAT_END)\n"
	@./scripts/clang-tidy.sh

#### Run tests #################################################################

run-glfw-app:
	$(PLATFORM_OUTPUT)/$(BUILDTYPE)/mapbox-glfw

run-valgrind-glfw-app:
	valgrind --leak-check=full --suppressions=../../../scripts/valgrind.sup $(PLATFORM_OUTPUT)/$(BUILDTYPE)/mapbox-glfw

ifneq (,$(shell command -v gdb))
  GDB = gdb -batch -return-child-result -ex 'set print thread-events off' -ex 'run' -ex 'thread apply all bt' --args
endif

test-%: Makefile/test
	$(GDB) $(PLATFORM_OUTPUT)/$(BUILDTYPE)/test --gtest_filter=$*

check: Makefile/test
	./scripts/collect-coverage.sh
