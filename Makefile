export BUILDTYPE ?= Debug
export IS_LOCAL_DEVELOPMENT ?= true
export WITH_CXX11ABI ?= $(shell scripts/check-cxx11abi.sh)
export TARGET_BRANCH ?= master


ifeq ($(BUILDTYPE), Release)
else ifeq ($(BUILDTYPE), RelWithDebInfo)
else ifeq ($(BUILDTYPE), Sanitize)
else ifeq ($(BUILDTYPE), Debug)
else
  $(error BUILDTYPE must be Debug, Sanitize, Release or RelWithDebInfo)
endif

buildtype := $(shell echo "$(BUILDTYPE)" | tr "[A-Z]" "[a-z]")

ifeq ($(shell uname -s), Darwin)
  HOST_PLATFORM = macos
  HOST_PLATFORM_VERSION = $(shell uname -m)
  export NINJA = platform/macos/ninja
  export JOBS ?= $(shell sysctl -n hw.ncpu)
else ifeq ($(shell uname -s), Linux)
  HOST_PLATFORM = linux
  HOST_PLATFORM_VERSION = $(shell uname -m)
  export NINJA = platform/linux/ninja
  export JOBS ?= $(shell grep --count processor /proc/cpuinfo)
else
  $(error Cannot determine host platform)
endif

ifeq ($(MASON_PLATFORM),)
  BUILD_PLATFORM = $(HOST_PLATFORM)
else
  BUILD_PLATFORM = $(MASON_PLATFORM)
endif

ifeq ($(MASON_PLATFORM_VERSION),)
  BUILD_PLATFORM_VERSION = $(HOST_PLATFORM_VERSION)
else
  BUILD_PLATFORM_VERSION = $(MASON_PLATFORM_VERSION)
endif

ifeq ($(MASON_PLATFORM),macos)
	MASON_PLATFORM=osx
endif

ifeq ($(V), 1)
  export XCPRETTY
  NINJA_ARGS ?= -v
else
  export XCPRETTY ?= | tee '$(shell pwd)/build/xcodebuild-$(shell date +"%Y-%m-%d_%H%M%S").log' | xcpretty
  NINJA_ARGS ?=
endif

.PHONY: default
default: test

BUILD_DEPS += Makefile
BUILD_DEPS += CMakeLists.txt

BUILD_DOCS ?= true

#### macOS targets ##############################################################

ifeq ($(HOST_PLATFORM), macos)

export PATH := $(shell pwd)/platform/macos:$(PATH)

MACOS_OUTPUT_PATH = build/macos
MACOS_PROJ_PATH = $(MACOS_OUTPUT_PATH)/mbgl.xcodeproj
MACOS_WORK_PATH = platform/macos/macos.xcworkspace
MACOS_USER_DATA_PATH = $(MACOS_WORK_PATH)/xcuserdata/$(USER).xcuserdatad
MACOS_COMPDB_PATH = $(MACOS_OUTPUT_PATH)/compdb/$(BUILDTYPE)

MACOS_XCODEBUILD = xcodebuild \
	-derivedDataPath $(MACOS_OUTPUT_PATH) \
	-configuration $(BUILDTYPE) \
	-workspace $(MACOS_WORK_PATH) \
	-jobs $(JOBS)

ifneq ($(CI),)
	MACOS_XCODEBUILD += -xcconfig platform/darwin/ci.xcconfig
endif

$(MACOS_PROJ_PATH): $(BUILD_DEPS) $(MACOS_USER_DATA_PATH)/WorkspaceSettings.xcsettings
	mkdir -p $(MACOS_OUTPUT_PATH)
	(cd $(MACOS_OUTPUT_PATH) && cmake -G Xcode ../.. \
		-DWITH_EGL=${WITH_EGL})

$(MACOS_USER_DATA_PATH)/WorkspaceSettings.xcsettings: platform/macos/WorkspaceSettings.xcsettings
	mkdir -p "$(MACOS_USER_DATA_PATH)"
	cp platform/macos/WorkspaceSettings.xcsettings "$@"

.PHONY: macos
macos: $(MACOS_PROJ_PATH)
	set -o pipefail && $(MACOS_XCODEBUILD) -scheme 'CI' build $(XCPRETTY)

.PHONY: xproj
xproj: $(MACOS_PROJ_PATH)
	xed $(MACOS_WORK_PATH)

.PHONY: test
test: $(MACOS_PROJ_PATH)
	set -o pipefail && $(MACOS_XCODEBUILD) -scheme 'mbgl-test' build $(XCPRETTY)

.PHONY: benchmark
benchmark: $(MACOS_PROJ_PATH)
	set -o pipefail && $(MACOS_XCODEBUILD) -scheme 'mbgl-benchmark' build $(XCPRETTY)

.PHONY: run-test
run-test: run-test-*

run-test-%: test
	ulimit -c unlimited && ($(MACOS_OUTPUT_PATH)/$(BUILDTYPE)/mbgl-test --gtest_catch_exceptions=0 --gtest_filter=$* & pid=$$! && wait $$pid \
	  || (lldb -c /cores/core.$$pid --batch --one-line 'thread backtrace all' --one-line 'quit' && exit 1))
	set -o pipefail && $(MACOS_XCODEBUILD) -scheme 'CI' test $(XCPRETTY)

.PHONY: run-benchmark
run-benchmark: run-benchmark-.

run-benchmark-%: benchmark
	$(MACOS_OUTPUT_PATH)/$(BUILDTYPE)/mbgl-benchmark --benchmark_filter=$* ${BENCHMARK_ARGS}

.PHONY: node-benchmark
node-benchmark: node

.PHONY: run-node-benchmark
run-node-benchmark: node-benchmark
	node platform/node/test/benchmark.js

.PHONY: glfw-app
glfw-app: $(MACOS_PROJ_PATH)
	set -o pipefail && $(MACOS_XCODEBUILD) -scheme 'mbgl-glfw' build $(XCPRETTY)

.PHONY: run-glfw-app
run-glfw-app: glfw-app
	"$(MACOS_OUTPUT_PATH)/$(BUILDTYPE)/mbgl-glfw"

.PHONY: render
render: $(MACOS_PROJ_PATH)
	set -o pipefail && $(MACOS_XCODEBUILD) -scheme 'mbgl-render' build $(XCPRETTY)

.PHONY: offline
offline: $(MACOS_PROJ_PATH)
	set -o pipefail && $(MACOS_XCODEBUILD) -scheme 'mbgl-offline' build $(XCPRETTY)

.PHONY: node
node: $(MACOS_PROJ_PATH)
	set -o pipefail && $(MACOS_XCODEBUILD) -scheme 'mbgl-node (Active ABI)' build $(XCPRETTY)

.PHONY: node-all
node-all: $(MACOS_PROJ_PATH)
	set -o pipefail && $(MACOS_XCODEBUILD) -scheme 'mbgl-node (All ABIs)' build $(XCPRETTY)

.PHONY: macos-test
macos-test: $(MACOS_PROJ_PATH)
	set -o pipefail && $(MACOS_XCODEBUILD) -scheme 'CI' test $(XCPRETTY)

.PHONY: macos-lint
macos-lint:
	find platform/macos -type f -name '*.plist' | xargs plutil -lint

.PHONY: xpackage
xpackage: $(MACOS_PROJ_PATH)
	SYMBOLS=$(SYMBOLS) ./platform/macos/scripts/package.sh

.PHONY: xdeploy
xdeploy:
	caffeinate -i ./platform/macos/scripts/deploy-packages.sh

.PHONY: xdocument
xdocument:
	OUTPUT=$(OUTPUT) ./platform/macos/scripts/document.sh

.PHONY: genstrings
genstrings:
	genstrings -u -o platform/macos/sdk/Base.lproj platform/darwin/src/*.{m,mm}
	genstrings -u -o platform/macos/sdk/Base.lproj platform/macos/src/*.{m,mm}
	genstrings -u -o platform/ios/resources/Base.lproj platform/ios/src/*.{m,mm}
	-find platform/ios/resources platform/macos/sdk -path '*/Base.lproj/*.strings' -exec \
		textutil -convert txt -extension strings -inputencoding UTF-16 -encoding UTF-8 {} \;
	mv platform/macos/sdk/Base.lproj/Foundation.strings platform/darwin/resources/Base.lproj/

$(MACOS_COMPDB_PATH)/Makefile:
	mkdir -p $(MACOS_COMPDB_PATH)
	(cd $(MACOS_COMPDB_PATH) && cmake ../../../.. \
		-DCMAKE_BUILD_TYPE=$(BUILDTYPE) \
		-DWITH_EGL=${WITH_EGL})

.PHONY:
compdb: $(BUILD_DEPS) $(TEST_DEPS) $(MACOS_COMPDB_PATH)/Makefile
	@$(MAKE) -C $(MACOS_COMPDB_PATH) cmake_check_build_system

.PHONY: tidy
tidy: compdb
	scripts/clang-tools.sh $(MACOS_COMPDB_PATH) $(TARGET_BRANCH)

.PHONY: check
check: compdb
	scripts/clang-tools.sh $(MACOS_COMPDB_PATH) $(TARGET_BRANCH) --diff
endif

#### iOS targets ##############################################################

ifeq ($(HOST_PLATFORM), macos)

IOS_OUTPUT_PATH = build/ios
IOS_PROJ_PATH = $(IOS_OUTPUT_PATH)/mbgl.xcodeproj
IOS_WORK_PATH = platform/ios/ios.xcworkspace
IOS_USER_DATA_PATH = $(IOS_WORK_PATH)/xcuserdata/$(USER).xcuserdatad

IOS_XCODEBUILD_SIM = xcodebuild \
	ARCHS=x86_64 ONLY_ACTIVE_ARCH=YES \
	-derivedDataPath $(IOS_OUTPUT_PATH) \
	-configuration $(BUILDTYPE) -sdk iphonesimulator \
	-workspace $(IOS_WORK_PATH) \
	-jobs $(JOBS)

ifneq ($(MORE_SIMULATORS),)
	IOS_LATEST = true
	IOS_11 = true
	IOS_10 = true
	IOS_9 = true
endif

ifdef IOS_LATEST
	IOS_XCODEBUILD_SIM += \
	-destination 'platform=iOS Simulator,OS=latest,name=iPhone 8' \
	-destination 'platform=iOS Simulator,OS=latest,name=iPhone Xs Max' \
	-destination 'platform=iOS Simulator,OS=latest,name=iPhone Xr' \
	-destination 'platform=iOS Simulator,OS=latest,name=iPad Pro (11-inch)'
endif

ifdef IOS_11
	IOS_XCODEBUILD_SIM += \
	-destination 'platform=iOS Simulator,OS=11.4,name=iPhone 7' \
	-destination 'platform=iOS Simulator,OS=11.4,name=iPhone X' \
	-destination 'platform=iOS Simulator,OS=11.4,name=iPad (5th generation)'
endif

ifdef IOS_10
	IOS_XCODEBUILD_SIM += \
	-destination 'platform=iOS Simulator,OS=10.3.1,name=iPhone SE' \
	-destination 'platform=iOS Simulator,OS=10.3.1,name=iPhone 7 Plus' \
	-destination 'platform=iOS Simulator,OS=10.3.1,name=iPad Pro (9.7-inch)'
endif

ifdef IOS_9
	IOS_XCODEBUILD_SIM += \
	-destination 'platform=iOS Simulator,OS=9.3,name=iPhone 6s Plus' \
	-destination 'platform=iOS Simulator,OS=9.3,name=iPhone 6s' \
	-destination 'platform=iOS Simulator,OS=9.3,name=iPad Air 2'
endif

# If IOS_XCODEBUILD_SIM does not contain a simulator destination, add the default.
ifeq (, $(findstring destination, $(IOS_XCODEBUILD_SIM)))
	IOS_XCODEBUILD_SIM += \
	-destination 'platform=iOS Simulator,OS=latest,name=iPhone 8'
else
	IOS_XCODEBUILD_SIM += -parallel-testing-enabled YES
endif

ifneq ($(ONLY_TESTING),)
	IOS_XCODEBUILD_SIM += -only-testing:$(ONLY_TESTING)
endif

ifneq ($(SKIP_TESTING),)
	IOS_XCODEBUILD_SIM += -skip-testing:$(SKIP_TESTING)
endif

ifneq ($(CI),)
	IOS_XCODEBUILD_SIM += -xcconfig platform/darwin/ci.xcconfig
endif

$(IOS_PROJ_PATH): $(IOS_USER_DATA_PATH)/WorkspaceSettings.xcsettings $(BUILD_DEPS)
	mkdir -p $(IOS_OUTPUT_PATH)
	(cd $(IOS_OUTPUT_PATH) && cmake -G Xcode ../.. \
		-DCMAKE_TOOLCHAIN_FILE=../../platform/ios/toolchain.cmake \
		-DMBGL_PLATFORM=ios \
		-DMASON_PLATFORM=ios)

$(IOS_USER_DATA_PATH)/WorkspaceSettings.xcsettings: platform/ios/WorkspaceSettings.xcsettings
	mkdir -p "$(IOS_USER_DATA_PATH)"
	cp platform/ios/WorkspaceSettings.xcsettings "$@"

.PHONY: ios
ios: $(IOS_PROJ_PATH)
	set -o pipefail && $(IOS_XCODEBUILD_SIM) -scheme 'CI' build $(XCPRETTY)

.PHONY: iproj
iproj: $(IOS_PROJ_PATH)
	xed $(IOS_WORK_PATH)

.PHONY: ios-lint
ios-lint: ios-pod-lint
	find platform/ios/framework -type f -name '*.plist' | xargs plutil -lint
	find platform/ios/app -type f -name '*.plist' | xargs plutil -lint

.PHONY: ios-pod-lint
ios-pod-lint:
	./platform/ios/scripts/lint-podspecs.js

.PHONY: ios-test
ios-test: $(IOS_PROJ_PATH)
	set -o pipefail && $(IOS_XCODEBUILD_SIM) -scheme 'CI' test $(XCPRETTY)

.PHONY: ios-integration-test
ios-integration-test: $(IOS_PROJ_PATH)
	set -o pipefail && $(IOS_XCODEBUILD_SIM) -scheme 'Integration Test Harness' test $(XCPRETTY)

.PHONY: ios-sanitize
ios-sanitize: $(IOS_PROJ_PATH)
	set -o pipefail && $(IOS_XCODEBUILD_SIM) -scheme 'CI' -enableThreadSanitizer YES -enableUndefinedBehaviorSanitizer YES test $(XCPRETTY)

.PHONY: ios-sanitize-address
ios-sanitize-address: $(IOS_PROJ_PATH)
	set -o pipefail && $(IOS_XCODEBUILD_SIM) -scheme 'CI' -enableAddressSanitizer YES test $(XCPRETTY)

.PHONY: ios-static-analyzer
ios-static-analyzer: $(IOS_PROJ_PATH)
	set -o pipefail && $(IOS_XCODEBUILD_SIM) analyze -scheme 'CI' test $(XCPRETTY)

.PHONY: ios-install-simulators
ios-install-simulators:
	xcversion simulators --install="iOS 11.4" || true
	xcversion simulators --install="iOS 10.3.1" || true
	xcversion simulators --install="iOS 9.3" || true

.PHONY: ios-check-events-symbols
ios-check-events-symbols:
	./platform/ios/scripts/check-events-symbols.sh

.PHONY: ipackage
ipackage: ipackage*
ipackage%:
	@echo make ipackage is deprecated — use make iframework.

.PHONY: iframework
iframework: $(IOS_PROJ_PATH)
	FORMAT=$(FORMAT) BUILD_DEVICE=$(BUILD_DEVICE) SYMBOLS=$(SYMBOLS) BUILD_DOCS=$(BUILD_DOCS) \
	./platform/ios/scripts/package.sh

.PHONY: ideploy
ideploy:
	caffeinate -i ./platform/ios/scripts/deploy-packages.sh

.PHONY: idocument
idocument:
	OUTPUT=$(OUTPUT) ./platform/ios/scripts/document.sh

.PHONY: darwin-style-code
darwin-style-code:
	node platform/darwin/scripts/generate-style-code.js
	node platform/darwin/scripts/update-examples.js
style-code: darwin-style-code

.PHONY: darwin-update-examples
darwin-update-examples:
	node platform/darwin/scripts/update-examples.js

.PHONY: darwin-check-public-symbols
darwin-check-public-symbols:
	node platform/darwin/scripts/check-public-symbols.js macOS iOS

endif

#### Linux targets #####################################################

ifeq ($(HOST_PLATFORM), linux)

export PATH := $(shell pwd)/platform/linux:$(PATH)
export LINUX_OUTPUT_PATH = build/linux-$(shell uname -m)/$(BUILDTYPE)
LINUX_BUILD = $(LINUX_OUTPUT_PATH)/build.ninja

$(LINUX_BUILD): $(BUILD_DEPS)
	mkdir -p $(LINUX_OUTPUT_PATH)
	(cd $(LINUX_OUTPUT_PATH) && cmake -G Ninja ../../.. \
		-DCMAKE_BUILD_TYPE=$(BUILDTYPE) \
		-DWITH_CXX11ABI=${WITH_CXX11ABI} \
		-DWITH_COVERAGE=${WITH_COVERAGE} \
		-DWITH_OSMESA=${WITH_OSMESA} \
		-DWITH_EGL=${WITH_EGL})

.PHONY: linux
linux: glfw-app render offline

.PHONY: linux-core
linux-core: $(LINUX_BUILD)
	$(NINJA) $(NINJA_ARGS) -j$(JOBS) -C $(LINUX_OUTPUT_PATH) mbgl-core mbgl-loop-uv mbgl-filesource

.PHONY: test
test: $(LINUX_BUILD)
	$(NINJA) $(NINJA_ARGS) -j$(JOBS) -C $(LINUX_OUTPUT_PATH) mbgl-test

.PHONY: benchmark
benchmark: $(LINUX_BUILD)
	$(NINJA) $(NINJA_ARGS) -j$(JOBS) -C $(LINUX_OUTPUT_PATH) mbgl-benchmark

GDB ?= gdb \
	-batch -return-child-result \
	-ex 'set print thread-events off' \
	-ex 'set disable-randomization off' \
	-ex 'run' \
	-ex 'thread apply all bt' --args

.PHONY: run-test
run-test: run-test-*

run-test-%: test
	$(GDB) $(LINUX_OUTPUT_PATH)/mbgl-test --gtest_catch_exceptions=0 --gtest_filter=$*

.PHONY: run-benchmark
run-benchmark: run-benchmark-.

run-benchmark-%: benchmark
	$(LINUX_OUTPUT_PATH)/mbgl-benchmark --benchmark_filter=$*

.PHONY: render
render: $(LINUX_BUILD)
	$(NINJA) $(NINJA_ARGS) -j$(JOBS) -C $(LINUX_OUTPUT_PATH) mbgl-render

.PHONY: offline
offline: $(LINUX_BUILD)
	$(NINJA) $(NINJA_ARGS) -j$(JOBS) -C $(LINUX_OUTPUT_PATH) mbgl-offline

.PHONY: glfw-app
glfw-app: $(LINUX_BUILD)
	$(NINJA) $(NINJA_ARGS) -j$(JOBS) -C $(LINUX_OUTPUT_PATH) mbgl-glfw

.PHONY: run-glfw-app
run-glfw-app: glfw-app
	cd $(LINUX_OUTPUT_PATH) && ./mbgl-glfw

.PHONY: node
node: $(LINUX_BUILD)
	$(NINJA) $(NINJA_ARGS) -j$(JOBS) -C $(LINUX_OUTPUT_PATH) mbgl-node.active

.PHONY: node-all
node-all: $(LINUX_BUILD)
	$(NINJA) $(NINJA_ARGS) -j$(JOBS) -C $(LINUX_OUTPUT_PATH) mbgl-node.all

.PHONY: compdb
compdb: $(LINUX_BUILD)
	# Ninja generator already outputs the file at the right location

.PHONY: tidy
tidy: compdb
	scripts/clang-tools.sh $(LINUX_OUTPUT_PATH) $(TARGET_BRANCH)

.PHONY: check
check: compdb
	scripts/clang-tools.sh $(LINUX_OUTPUT_PATH) $(TARGET_BRANCH) --diff

endif

#### Qt targets #####################################################

QT_QMAKE_FOUND := $(shell command -v qmake 2> /dev/null)
ifdef QT_QMAKE_FOUND
  export QT_INSTALL_DOCS = $(shell qmake -query QT_INSTALL_DOCS)
  QT_ROOT_PATH = build/qt-$(BUILD_PLATFORM)-$(BUILD_PLATFORM_VERSION)
endif

export QT_OUTPUT_PATH = $(QT_ROOT_PATH)/$(BUILDTYPE)
QT_BUILD = $(QT_OUTPUT_PATH)/build.ninja

$(QT_BUILD): $(BUILD_DEPS)
	@scripts/check-qt.sh
	mkdir -p $(QT_OUTPUT_PATH)
	(cd $(QT_OUTPUT_PATH) && cmake -G Ninja ../../.. \
		-DCMAKE_BUILD_TYPE=$(BUILDTYPE) \
		-DMBGL_PLATFORM=qt \
		-DMASON_PLATFORM=$(MASON_PLATFORM) \
		-DMASON_PLATFORM_VERSION=$(MASON_PLATFORM_VERSION) \
		-DWITH_QT_DECODERS=${WITH_QT_DECODERS} \
		-DWITH_QT_I18N=${WITH_QT_I18N} \
		-DWITH_CXX11ABI=${WITH_CXX11ABI} \
		-DWITH_COVERAGE=${WITH_COVERAGE})

ifeq ($(HOST_PLATFORM), macos)

MACOS_QT_PROJ_PATH = $(QT_ROOT_PATH)/xcode/mbgl.xcodeproj
$(MACOS_QT_PROJ_PATH): $(BUILD_DEPS)
	@scripts/check-qt.sh
	mkdir -p $(QT_ROOT_PATH)/xcode
	(cd $(QT_ROOT_PATH)/xcode && cmake -G Xcode ../../.. \
		-DMBGL_PLATFORM=qt \
		-DMASON_PLATFORM=$(MASON_PLATFORM) \
		-DMASON_PLATFORM_VERSION=$(MASON_PLATFORM_VERSION) \
		-DWITH_QT_DECODERS=${WITH_QT_DECODERS} \
		-DWITH_QT_I18N=${WITH_QT_I18N} \
		-DWITH_CXX11ABI=${WITH_CXX11ABI} \
		-DWITH_COVERAGE=${WITH_COVERAGE})

.PHONY: qtproj
qtproj: $(MACOS_QT_PROJ_PATH)
	open $(MACOS_QT_PROJ_PATH)

endif

ifdef QNX_HOST
export WITH_QT_DECODERS ?= ON
export QCC_COMPILER_TARGET ?= gcc_ntox86_64
export QCC_NTOARCH ?= x86_64

export QNX_OUTPUT_PATH = build/qt-qnx-$(QCC_NTOARCH)/$(BUILDTYPE)
QNX_QT_BUILD = $(QNX_OUTPUT_PATH)/build.ninja
$(QNX_QT_BUILD): $(BUILD_DEPS)
	@scripts/check-qt.sh
	mkdir -p $(QNX_OUTPUT_PATH)
	(cd $(QNX_OUTPUT_PATH) && cmake -G Ninja ../../.. \
		-DCMAKE_BUILD_TYPE=$(BUILDTYPE) \
		-DQCC_COMPILER_TARGET=${QCC_COMPILER_TARGET} \
		-DQCC_NTOARCH=${QCC_NTOARCH} \
		-DCMAKE_TOOLCHAIN_FILE=platform/qt/qnx.cmake \
		-DMBGL_PLATFORM=qt \
		-DWITH_QT_DECODERS=${WITH_QT_DECODERS} \
		-DWITH_QT_I18N=${WITH_QT_I18N} \
		-DWITH_CXX11ABI=${WITH_CXX11ABI} \
		-DWITH_COVERAGE=${WITH_COVERAGE})

.PHONY: qnx-qt-lib
qnx-qt-lib: $(QNX_QT_BUILD)
	$(NINJA) $(NINJA_ARGS) -j$(JOBS) -C $(QNX_OUTPUT_PATH) qmapboxgl

endif

.PHONY: qt-lib
qt-lib: $(QT_BUILD)
	$(NINJA) $(NINJA_ARGS) -j$(JOBS) -C $(QT_OUTPUT_PATH) qmapboxgl

.PHONY: qt-app
qt-app: $(QT_BUILD)
	$(NINJA) $(NINJA_ARGS) -j$(JOBS) -C $(QT_OUTPUT_PATH) mbgl-qt

.PHONY: run-qt-app
run-qt-app: qt-app
	$(QT_OUTPUT_PATH)/mbgl-qt

.PHONY: qt-test
qt-test: $(QT_BUILD)
	$(NINJA) $(NINJA_ARGS) -j$(JOBS) -C $(QT_OUTPUT_PATH) mbgl-test

run-qt-test-%: qt-test
	$(QT_OUTPUT_PATH)/mbgl-test --gtest_catch_exceptions=0 --gtest_filter=$*

.PHONY: run-qt-test
run-qt-test: run-qt-test-*

.PHONY: qt-docs
qt-docs:
	qdoc $(shell pwd)/platform/qt/config.qdocconf -outputdir $(shell pwd)/$(QT_OUTPUT_PATH)/docs

#### Node targets ##############################################################

.PHONY: test-node
test-node: node
	npm test
	npm run test-query
	npm run test-expressions

#### Android targets ###########################################################

MBGL_ANDROID_ABIS  = arm-v7;armeabi-v7a
MBGL_ANDROID_ABIS += arm-v8;arm64-v8a
MBGL_ANDROID_ABIS += x86;x86
MBGL_ANDROID_ABIS += x86-64;x86_64

MBGL_ANDROID_LOCAL_WORK_DIR = /data/local/tmp/core-tests
MBGL_ANDROID_LOCAL_BENCHMARK_DIR = /data/local/tmp/benchmark
MBGL_ANDROID_LIBDIR = lib$(if $(filter arm-v8 x86-64,$1),64)
MBGL_ANDROID_DALVIKVM = dalvikvm$(if $(filter arm-v8 x86-64,$1),64,32)
MBGL_ANDROID_APK_SUFFIX = $(if $(filter Release,$(BUILDTYPE)),release,debug)
MBGL_ANDROID_CORE_TEST_DIR = platform/android/MapboxGLAndroidSDK/.externalNativeBuild/cmake/$(buildtype)/$2/core-tests
MBGL_ANDROID_BENCHMARK_DIR = platform/android/MapboxGLAndroidSDK/.externalNativeBuild/cmake/$(buildtype)/$2/benchmark
MBGL_ANDROID_STL ?= c++_static
MBGL_ANDROID_GRADLE = ./gradlew --parallel --max-workers=$(JOBS) -Pmapbox.buildtype=$(buildtype) -Pmapbox.stl=$(MBGL_ANDROID_STL)

# Lists all devices, and extracts the identifiers, then obtains the ABI for every one.
# Some devices return \r\n, so we'll have to remove the carriage return before concatenating.
MBGL_ANDROID_ACTIVE_ARCHS = $(shell adb devices | sed '1d;/^\*/d;s/[[:space:]].*//' | xargs -n 1 -I DEV `type -P adb` -s DEV shell getprop ro.product.cpu.abi | tr -d '\r')

# Generate code based on the style specification
.PHONY: android-style-code
android-style-code:
	node platform/android/scripts/generate-style-code.js
style-code: android-style-code

# Configuration file for running CMake from Gradle within Android Studio.
platform/android/gradle/configuration.gradle:
	@printf "ext {\n    node = '`command -v node || command -v nodejs`'\n    npm = '`command -v npm`'\n    ccache = '`command -v ccache`'\n}" > $@

define ANDROID_RULES
# $1 = arm-v7 (short arch)
# $2 = armeabi-v7a (internal arch)

.PHONY: android-test-lib-$1
android-test-lib-$1: platform/android/gradle/configuration.gradle
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=$2 -Pmapbox.with_test=true :MapboxGLAndroidSDKTestApp:assemble$(BUILDTYPE)

.PHONY: android-benchmark-$1
android-benchmark-$1: platform/android/gradle/configuration.gradle
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=$2 -Pmapbox.with_benchmark=true :MapboxGLAndroidSDKTestApp:assemble$(BUILDTYPE)

# Build SDK for for specified abi
.PHONY: android-lib-$1
android-lib-$1: platform/android/gradle/configuration.gradle
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=$2 :MapboxGLAndroidSDK:assemble$(BUILDTYPE)

# Build test app and SDK for for specified abi
.PHONY: android-$1
android-$1: platform/android/gradle/configuration.gradle
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=$2 :MapboxGLAndroidSDKTestApp:assemble$(BUILDTYPE)

# Build the core test for specified abi
.PHONY: android-core-test-$1
android-core-test-$1: android-test-lib-$1
	# Compile main sources and extract the classes (using the test app to get all transitive dependencies in one place)
	mkdir -p $(MBGL_ANDROID_CORE_TEST_DIR)
	unzip -o platform/android/MapboxGLAndroidSDKTestApp/build/outputs/apk/$(buildtype)/MapboxGLAndroidSDKTestApp-$(MBGL_ANDROID_APK_SUFFIX).apk classes.dex -d $(MBGL_ANDROID_CORE_TEST_DIR)

run-android-core-test-$1-%: android-core-test-$1
	# Ensure clean state on the device
	adb shell "rm -Rf $(MBGL_ANDROID_LOCAL_WORK_DIR) && mkdir -p $(MBGL_ANDROID_LOCAL_WORK_DIR)/test && mkdir -p $(MBGL_ANDROID_LOCAL_WORK_DIR)/mapbox-gl-js/src/style-spec/reference"

	# Push all needed files to the device
	adb push $(MBGL_ANDROID_CORE_TEST_DIR)/classes.dex $(MBGL_ANDROID_LOCAL_WORK_DIR) > /dev/null 2>&1
	adb push platform/android/MapboxGLAndroidSDK/build/intermediates/intermediate-jars/$(buildtype)/jni/$2/libmapbox-gl.so $(MBGL_ANDROID_LOCAL_WORK_DIR) > /dev/null 2>&1
	adb push test/fixtures $(MBGL_ANDROID_LOCAL_WORK_DIR)/test > /dev/null 2>&1
	adb push mapbox-gl-js/src/style-spec/reference/v8.json $(MBGL_ANDROID_LOCAL_WORK_DIR)/mapbox-gl-js/src/style-spec/reference > /dev/null 2>&1
	adb push platform/android/MapboxGLAndroidSDK/build/intermediates/cmake/$(buildtype)/obj/$2/mbgl-test $(MBGL_ANDROID_LOCAL_WORK_DIR) > /dev/null 2>&1

# Create gtest filter for skipped tests.
	$(eval SKIPPED_TESTS := -$(shell sed -n '/#\|^$$/!p' platform/android/tests/skipped.txt | sed ':a;$!N;s/\n/:/g;ta'))

	# Kick off the tests
	adb shell "export LD_LIBRARY_PATH=$(MBGL_ANDROID_LOCAL_WORK_DIR) && cd $(MBGL_ANDROID_LOCAL_WORK_DIR) && chmod +x mbgl-test && ./mbgl-test --class_path=$(MBGL_ANDROID_LOCAL_WORK_DIR)/classes.dex --gtest_filter=$$*:$(SKIPPED_TESTS)"

	# Gather the results and unpack them
	adb shell "cd $(MBGL_ANDROID_LOCAL_WORK_DIR) && tar -cvzf results.tgz test/fixtures/*  > /dev/null 2>&1"
	adb pull $(MBGL_ANDROID_LOCAL_WORK_DIR)/results.tgz $(MBGL_ANDROID_CORE_TEST_DIR)/ > /dev/null 2>&1
	rm -rf $(MBGL_ANDROID_CORE_TEST_DIR)/results && mkdir -p $(MBGL_ANDROID_CORE_TEST_DIR)/results
	tar -xzf $(MBGL_ANDROID_CORE_TEST_DIR)/results.tgz --strip-components=2 -C $(MBGL_ANDROID_CORE_TEST_DIR)/results

# Run the core test for specified abi
.PHONY: run-android-core-test-$1
run-android-core-test-$1: run-android-core-test-$1-*

# Run benchmarks for specified abi
.PHONY: run-android-benchmark-$1
run-android-benchmark-$1: run-android-benchmark-$1-*

run-android-benchmark-$1-%: android-benchmark-$1
	mkdir -p $(MBGL_ANDROID_BENCHMARK_DIR)
	unzip -o platform/android/MapboxGLAndroidSDKTestApp/build/outputs/apk/$(buildtype)/MapboxGLAndroidSDKTestApp-$(MBGL_ANDROID_APK_SUFFIX).apk classes.dex -d $(MBGL_ANDROID_BENCHMARK_DIR)

	# Delete old test folder and create new one
	adb shell "rm -Rf $(MBGL_ANDROID_LOCAL_BENCHMARK_DIR) && mkdir -p $(MBGL_ANDROID_LOCAL_BENCHMARK_DIR)/benchmark && mkdir -p $(MBGL_ANDROID_LOCAL_BENCHMARK_DIR)/test"

	# Push compiled java sources, test data and executable to device
	adb push $(MBGL_ANDROID_BENCHMARK_DIR)/classes.dex $(MBGL_ANDROID_LOCAL_BENCHMARK_DIR) > /dev/null 2>&1
	adb push platform/android/MapboxGLAndroidSDK/build/intermediates/intermediate-jars/$(buildtype)/jni/$2/libmapbox-gl.so $(MBGL_ANDROID_LOCAL_BENCHMARK_DIR) > /dev/null 2>&1
	adb push benchmark/fixtures $(MBGL_ANDROID_LOCAL_BENCHMARK_DIR)/benchmark > /dev/null 2>&1
	adb push test/fixtures $(MBGL_ANDROID_LOCAL_BENCHMARK_DIR)/test > /dev/null 2>&1
	adb push platform/android/MapboxGLAndroidSDK/build/intermediates/cmake/$(buildtype)/obj/$2/mbgl-benchmark $(MBGL_ANDROID_LOCAL_BENCHMARK_DIR) > /dev/null 2>&1

	# Run benchmark. Number of benchmark iterations can be set by run-android-benchmark-N parameter.
	adb shell "export LD_LIBRARY_PATH=$(MBGL_ANDROID_LOCAL_BENCHMARK_DIR) && cd $(MBGL_ANDROID_LOCAL_BENCHMARK_DIR) && chmod +x mbgl-benchmark && ./mbgl-benchmark --class_path=$(MBGL_ANDROID_LOCAL_BENCHMARK_DIR)/classes.dex --benchmark_repetitions=$$* --benchmark_format=json --benchmark_out=results.json"

	# Pull results.json from the device
	rm -rf $(MBGL_ANDROID_BENCHMARK_DIR)/results && mkdir -p $(MBGL_ANDROID_BENCHMARK_DIR)/results
	adb pull $(MBGL_ANDROID_LOCAL_BENCHMARK_DIR)/results.json $(MBGL_ANDROID_BENCHMARK_DIR)/results > /dev/null 2>&1

# Run the test app on connected android device with specified abi
.PHONY: run-android-$1
run-android-$1: platform/android/gradle/configuration.gradle
	-adb uninstall com.mapbox.mapboxsdk.testapp 2> /dev/null
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=$2 :MapboxGLAndroidSDKTestApp:install$(BUILDTYPE) && adb shell am start -n com.mapbox.mapboxsdk.testapp/.activity.FeatureOverviewActivity

# Build test app instrumentation tests apk and test app apk for specified abi
.PHONY: android-ui-test-$1
android-ui-test-$1: platform/android/gradle/configuration.gradle
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=$2 :MapboxGLAndroidSDKTestApp:assembleDebug :MapboxGLAndroidSDKTestApp:assembleAndroidTest

# Run test app instrumentation tests on a connected android device or emulator with specified abi
.PHONY: run-android-ui-test-$1
run-android-ui-test-$1: platform/android/gradle/configuration.gradle
	-adb uninstall com.mapbox.mapboxsdk.testapp 2> /dev/null
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=$2 :MapboxGLAndroidSDKTestApp:connectedAndroidTest

# Run Java Instrumentation tests on a connected android device or emulator with specified abi and test filter
run-android-ui-test-$1-%: platform/android/gradle/configuration.gradle
	-adb uninstall com.mapbox.mapboxsdk.testapp 2> /dev/null
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=$2 :MapboxGLAndroidSDKTestApp:connectedAndroidTest -Pandroid.testInstrumentationRunnerArguments.class="$$*"

# Symbolicate native stack trace with the specified abi
.PHONY: android-ndk-stack-$1
android-ndk-stack-$1: platform/android/gradle/configuration.gradle
	adb logcat | ndk-stack -sym platform/android/MapboxGLAndroidSDK/build/intermediates/cmake/debug/obj/$2/

# Run render tests with pixelmatch
.PHONY: run-android-render-test-$1
run-android-render-test-$1: $(BUILD_DEPS) platform/android/gradle/configuration.gradle
	-adb uninstall com.mapbox.mapboxsdk.testapp 2> /dev/null
	# delete old test results
	rm -rf platform/android/build/render-test/mapbox/
  # copy test definitions & ignore file to test app assets folder, clear old ones first
	rm -rf platform/android/MapboxGLAndroidSDKTestApp/src/main/assets/integration
	cp -r mapbox-gl-js/test/integration platform/android/MapboxGLAndroidSDKTestApp/src/main/assets
	cp platform/node/test/ignores.json platform/android/MapboxGLAndroidSDKTestApp/src/main/assets/integration/ignores.json
	# run RenderTest.java to generate static map images
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=$2 :MapboxGLAndroidSDKTestApp:connectedAndroidTest -Pandroid.testInstrumentationRunnerArguments.class="com.mapbox.mapboxsdk.testapp.render.RenderTest"
	# pull generated images from the device
	adb pull "`adb shell 'printenv EXTERNAL_STORAGE' | tr -d '\r'`/mapbox/render" platform/android/build/render-test
	# copy expected result and run pixelmatch
	python platform/android/scripts/run-render-test.py
	# remove test definitions from assets
	rm -rf platform/android/MapboxGLAndroidSDKTestApp/src/main/assets/integration

endef

# Explodes the arguments into individual variables
define ANDROID_RULES_INVOKER
$(call ANDROID_RULES,$(word 1,$1),$(word 2,$1))
endef

$(foreach abi,$(MBGL_ANDROID_ABIS),$(eval $(call ANDROID_RULES_INVOKER,$(subst ;, ,$(abi)))))

# Build the Android SDK and test app with abi set to arm-v7
.PHONY: android
android: android-arm-v7

# Build the Android SDK with abi set to arm-v7
.PHONY: android-lib
android-lib: android-lib-arm-v7

# Run the test app on connected android device with abi set to arm-v7
.PHONY: run-android
run-android: run-android-arm-v7

# Run Java Instrumentation tests on a connected android device or emulator with abi set to arm-v7
.PHONY: run-android-ui-test
run-android-ui-test: run-android-ui-test-arm-v7
run-android-ui-test-%: run-android-ui-test-arm-v7-%

# Run Java Unit tests on the JVM of the development machine executing this
.PHONY: run-android-unit-test
run-android-unit-test: platform/android/gradle/configuration.gradle
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=none :MapboxGLAndroidSDK:testDebugUnitTest --info
run-android-unit-test-%: platform/android/gradle/configuration.gradle
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=none :MapboxGLAndroidSDK:testDebugUnitTest  --info --tests "$*"

# Run unit test and build a coverage report from .exec file generated by unit tests and .ec file generated by instrumentation tests
.PHONY: android-create-jacoco-report
android-create-jacoco-report: platform/android/gradle/configuration.gradle
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=none :MapboxGLAndroidSDK:jacocoTestReport

# Parse merged jacoco report and send it to S3
.PHONY: android-parse-and-send-jacoco-report
android-parse-and-send-jacoco-report:
	python platform/android/scripts/parse-jacoco-report.py

# Builds a release package of the Android SDK
.PHONY: apackage
apackage: platform/android/gradle/configuration.gradle
	make android-lib-arm-v7 && make android-lib-arm-v8 && make android-lib-x86 && make android-lib-x86-64
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=all assemble$(BUILDTYPE)

# Build test app instrumentation tests apk and test app apk for all abi's
.PHONY: android-ui-test
android-ui-test: platform/android/gradle/configuration.gradle
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=all :MapboxGLAndroidSDKTestApp:assembleDebug :MapboxGLAndroidSDKTestApp:assembleAndroidTest

#Run instrumentations tests on MicroSoft App Center
.PHONY: run-android-test-app-center
run-android-test-app-center:
	cd platform/android && appcenter test run espresso --app "mapboxcn-outlook.com/MapsSdk" --devices "mapboxcn-outlook.com/china" --app-path MapboxGLAndroidSDKTestApp/build/outputs/apk/debug/MapboxGLAndroidSDKTestApp-debug.apk  --test-series "master" --locale "en_US" --build-dir MapboxGLAndroidSDKTestApp/build/outputs/apk/androidTest/debug

# Uploads the compiled Android SDK to Bintray
.PHONY: run-android-upload-to-bintray
run-android-upload-to-bintray: platform/android/gradle/configuration.gradle
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=all :MapboxGLAndroidSDK:bintrayUpload

# Uploads the compiled Android SDK SNAPSHOT to oss.jfrog.org
.PHONY: run-android-upload-to-artifactory
run-android-upload-to-artifactory: platform/android/gradle/configuration.gradle
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=all :MapboxGLAndroidSDK:artifactoryPublish

# Dump system graphics information for the test app
.PHONY: android-gfxinfo
android-gfxinfo:
	adb shell dumpsys gfxinfo com.mapbox.mapboxsdk.testapp reset

# Generates Activity sanity tests
.PHONY: test-code-android
test-code-android:
	node platform/android/scripts/generate-test-code.js

# Runs checkstyle and lint on the java code
.PHONY: android-check
android-check : android-checkstyle run-android-nitpick android-lint-sdk android-lint-test-app android-ktlint

# Runs checkstyle on the java code
.PHONY: android-checkstyle
android-checkstyle: platform/android/gradle/configuration.gradle
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=none :MapboxGLAndroidSDK:checkstyle :MapboxGLAndroidSDKTestApp:checkstyle

# Runs checkstyle on the kotlin code
.PHONY: android-ktlint
android-ktlint: 
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=none ktlint

# Runs lint on the Android SDK java code
.PHONY: android-lint-sdk
android-lint-sdk: platform/android/gradle/configuration.gradle
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=none :MapboxGLAndroidSDK:lint

# Runs lint on the Android test app java code
.PHONY: android-lint-test-app
android-lint-test-app: platform/android/gradle/configuration.gradle
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=none :MapboxGLAndroidSDKTestApp:lint

# Generates javadoc from the Android SDK
.PHONY: android-javadoc
android-javadoc: platform/android/gradle/configuration.gradle
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=none :MapboxGLAndroidSDK:javadocrelease

# Generates platform/android/LICENSE.md file based on all Android project dependencies
.PHONY: android-license
android-license: platform/android/gradle/configuration.gradle
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=none :MapboxGLAndroidSDK:licenseReleaseReport
	python platform/android/scripts/generate-license.py

# Symbolicate ndk stack traces for the arm-v7 abi
.PHONY: android-ndk-stack
android-ndk-stack: android-ndk-stack-arm-v7

# Open Android Studio if machine is macos
ifeq ($(HOST_PLATFORM), macos)
.PHONY: aproj
aproj: platform/android/gradle/configuration.gradle
	open -b com.google.android.studio platform/android
endif

# Creates the configuration needed to build with Android Studio
.PHONY: android-configuration
android-configuration: platform/android/gradle/configuration.gradle
	cat platform/android/gradle/configuration.gradle

# Updates Android's vendor submodules
.PHONY: android-update-vendor
android-update-vendor: platform/android/gradle/configuration.gradle
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=none updateVendorSubmodules

# Run android nitpick script
.PHONY: run-android-nitpick
run-android-nitpick: android-update-vendor
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=none androidNitpick

# Creates a dependency graph using Graphviz
.PHONY: android-graph
android-graph: platform/android/gradle/configuration.gradle
	cd platform/android && $(MBGL_ANDROID_GRADLE) -Pmapbox.abis=none :MapboxGLAndroidSDK:generateDependencyGraphMapboxLibraries

#### Miscellaneous targets #####################################################

.PHONY: style-code
style-code:
	node scripts/generate-style-code.js
	node scripts/generate-shaders.js

.PHONY: codestyle
codestyle:
	scripts/codestyle.sh

.PHONY: clean
clean:
	-rm -rf ./build \
	        ./lib/*.node \
	        ./platform/android/gradle/configuration.gradle \
	        ./platform/android/MapboxGLAndroidSDK/build \
	        ./platform/android/MapboxGLAndroidSDK/.externalNativeBuild \
	        ./platform/android/MapboxGLAndroidSDKTestApp/build \
	        ./platform/android/MapboxGLAndroidSDKTestApp/src/androidTest/java/com/mapbox/mapboxsdk/testapp/activity/gen \
	        ./platform/android/MapboxGLAndroidSDK/src/main/assets \
		./platform/android/MapboxGLAndroidSDKTestApp/src/main/assets/integration

.PHONY: distclean
distclean: clean
	-rm -rf ./mason_packages
	-rm -rf ./node_modules
