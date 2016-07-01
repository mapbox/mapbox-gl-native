export BUILDTYPE ?= Debug

ifeq ($(shell uname -s), Darwin)
  HOST_PLATFORM = macos
  HOST_PLATFORM_VERSION = $(shell uname -m)
  export JOBS ?= $(shell sysctl -n hw.ncpu)
else ifeq ($(shell uname -s), Linux)
  HOST_PLATFORM = linux
  HOST_PLATFORM_VERSION = $(shell uname -m)
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

ifeq ($(V), 1)
  export XCPRETTY
else
  export XCPRETTY ?= | xcpretty
endif

default: test-$(BUILD_PLATFORM)

ifneq (,$(wildcard .git/.))
.mason/mason:
	git submodule update --init
else
.mason/mason: ;
endif

.NOTPARALLEL: node_modules
node_modules: package.json
	npm update # Install dependencies but don't run our own install script.

GYP = deps/run_gyp --depth=. -Goutput_dir=.

CONFIG_DEPENDENCIES = .mason/mason configure

# Depend on gyp includes plus directories, so that projects are regenerated when
# files are added or removed.
GYP_DEPENDENCIES = mbgl.gypi test/test.gypi benchmark/benchmark.gypi bin/*.gypi $(shell find src include -type d) node_modules

#### macOS targets ##############################################################

MACOS_OUTPUT_PATH = build/macos
MACOS_PROJ_PATH = $(MACOS_OUTPUT_PATH)/platform/macos/platform.xcodeproj
MACOS_WORK_PATH = platform/macos/macos.xcworkspace
MACOS_USER_DATA_PATH = $(MACOS_WORK_PATH)/xcuserdata/$(USER).xcuserdatad

$(MACOS_OUTPUT_PATH)/config.gypi: platform/macos/scripts/configure.sh $(CONFIG_DEPENDENCIES)
	./configure $< $@ macos

$(MACOS_OUTPUT_PATH)/mbgl.xcconfig: $(MACOS_OUTPUT_PATH)/config.gypi
	./scripts/export-xcconfig.py $< $@

$(MACOS_PROJ_PATH): platform/macos/platform.gyp $(MACOS_OUTPUT_PATH)/config.gypi $(MACOS_OUTPUT_PATH)/mbgl.xcconfig $(GYP_DEPENDENCIES)
	$(GYP) -f xcode --generator-output=$(MACOS_OUTPUT_PATH) $<

macos: $(MACOS_PROJ_PATH)
	set -o pipefail && xcodebuild \
	  -derivedDataPath $(MACOS_OUTPUT_PATH) \
	  -configuration $(BUILDTYPE) \
	  -workspace $(MACOS_WORK_PATH) -scheme CI build $(XCPRETTY)

xproj: $(MACOS_PROJ_PATH) $(MACOS_WORK_PATH)
	mkdir -p "$(MACOS_USER_DATA_PATH)"
	cp platform/macos/WorkspaceSettings.xcsettings "$(MACOS_USER_DATA_PATH)/WorkspaceSettings.xcsettings"
	open $(MACOS_WORK_PATH)

test-macos: macos node_modules
	ulimit -c unlimited && ($(MACOS_OUTPUT_PATH)/Build/Products/$(BUILDTYPE)/test & pid=$$! && wait $$pid \
	  || (lldb -c /cores/core.$$pid --batch --one-line 'thread backtrace all' --one-line 'quit' && exit 1))
	set -o pipefail && xcodebuild \
	  -derivedDataPath $(MACOS_OUTPUT_PATH) \
	  -configuration $(BUILDTYPE) \
	  -workspace $(MACOS_WORK_PATH) -scheme CI test $(XCPRETTY)

xpackage: $(MACOS_PROJ_PATH)
	SYMBOLS=$(SYMBOLS) ./platform/macos/scripts/package.sh

xdocument:
	OUTPUT=$(OUTPUT) ./platform/macos/scripts/document.sh

genstrings:
	genstrings -u -o platform/macos/sdk/Base.lproj platform/darwin/src/*.{m,mm}
	genstrings -u -o platform/macos/sdk/Base.lproj platform/macos/src/*.{m,mm}
	genstrings -u -o platform/ios/resources/Base.lproj platform/ios/src/*.{m,mm}
	-find platform/ios/resources platform/macos/sdk -path '*/Base.lproj/*.strings' -exec \
		textutil -convert txt -extension strings -inputencoding UTF-16 -encoding UTF-8 {} \;
	mv platform/macos/sdk/Base.lproj/Foundation.strings platform/darwin/resources/Base.lproj/

#### iOS targets ##############################################################

IOS_OUTPUT_PATH = build/ios
IOS_PROJ_PATH = $(IOS_OUTPUT_PATH)/platform/ios/platform.xcodeproj
IOS_WORK_PATH = platform/ios/ios.xcworkspace
IOS_USER_DATA_PATH = $(IOS_WORK_PATH)/xcuserdata/$(USER).xcuserdatad

$(IOS_OUTPUT_PATH)/config.gypi: platform/ios/scripts/configure.sh $(CONFIG_DEPENDENCIES)
	./configure $< $@ ios

$(IOS_OUTPUT_PATH)/mbgl.xcconfig: $(IOS_OUTPUT_PATH)/config.gypi
	./scripts/export-xcconfig.py $< $@

$(IOS_PROJ_PATH): platform/ios/platform.gyp $(IOS_OUTPUT_PATH)/config.gypi $(IOS_OUTPUT_PATH)/mbgl.xcconfig $(GYP_DEPENDENCIES)
	$(GYP) -f xcode --generator-output=$(IOS_OUTPUT_PATH) $<

ios: $(IOS_PROJ_PATH)
	set -o pipefail && xcodebuild \
	  ARCHS=x86_64 ONLY_ACTIVE_ARCH=YES \
	  -derivedDataPath $(IOS_OUTPUT_PATH) \
	  -configuration $(BUILDTYPE) -sdk iphonesimulator \
	  -destination 'platform=iOS Simulator,name=iPhone 6,OS=latest' \
	  -workspace $(IOS_WORK_PATH) -scheme CI build $(XCPRETTY)

iproj: $(IOS_PROJ_PATH)
	mkdir -p "$(IOS_USER_DATA_PATH)"
	cp platform/ios/WorkspaceSettings.xcsettings "$(IOS_USER_DATA_PATH)/WorkspaceSettings.xcsettings"
	open $(IOS_WORK_PATH)

test-ios: ios
	ios-sim start --devicetypeid 'com.apple.CoreSimulator.SimDeviceType.iPhone-6,9.3'
	ios-sim launch $(IOS_OUTPUT_PATH)/Build/Products/$(BUILDTYPE)-iphonesimulator/ios-test.app --verbose --devicetypeid 'com.apple.CoreSimulator.SimDeviceType.iPhone-6,9.3'
	set -o pipefail && xcodebuild \
	  ARCHS=x86_64 ONLY_ACTIVE_ARCH=YES \
	  -derivedDataPath $(IOS_OUTPUT_PATH) \
	  -configuration $(BUILDTYPE) -sdk iphonesimulator \
	  -destination 'platform=iOS Simulator,name=iPhone 6,OS=latest' \
	  -workspace $(IOS_WORK_PATH) -scheme CI test $(XCPRETTY)

ipackage: $(IOS_PROJ_PATH)
	BITCODE=$(BITCODE) FORMAT=$(FORMAT) BUILD_DEVICE=$(BUILD_DEVICE) SYMBOLS=$(SYMBOLS) \
	./platform/ios/scripts/package.sh

ipackage-strip: $(IOS_PROJ_PATH)
	BITCODE=$(BITCODE) FORMAT=$(FORMAT) BUILD_DEVICE=$(BUILD_DEVICE) SYMBOLS=NO \
	./platform/ios/scripts/package.sh

ipackage-sim: $(IOS_PROJ_PATH)
	BUILDTYPE=Debug BITCODE=$(BITCODE) FORMAT=dynamic BUILD_DEVICE=false SYMBOLS=$(SYMBOLS) \
	./platform/ios/scripts/package.sh

iframework: $(IOS_PROJ_PATH)
	BITCODE=$(BITCODE) FORMAT=dynamic BUILD_DEVICE=$(BUILD_DEVICE) SYMBOLS=$(SYMBOLS) \
	./platform/ios/scripts/package.sh

ifabric: $(IOS_PROJ_PATH)
	BITCODE=$(BITCODE) FORMAT=static BUILD_DEVICE=$(BUILD_DEVICE) SYMBOLS=NO SELF_CONTAINED=YES \
	./platform/ios/scripts/package.sh

idocument:
	OUTPUT=$(OUTPUT) ./platform/ios/scripts/document.sh

#### Android targets #####################################################

ANDROID_ENV = platform/android/scripts/toolchain.sh
ANDROID_ABIS = arm-v5 arm-v7 arm-v8 x86 x86-64 mips
ANDROID_LOCAL_WORK_DIR = /data/local/tmp/core-tests
ANDROID_DX := $(shell command -v dx 2> /dev/null)

define ANDROID_RULES
build/android-$1/config.gypi: platform/android/scripts/configure.sh $(CONFIG_DEPENDENCIES)
	$$(shell $(ANDROID_ENV) $1) ./configure $$< $$@ android $1

build/android-$1/Makefile: platform/android/platform.gyp build/android-$1/config.gypi $(GYP_DEPENDENCIES)
	$$(shell $(ANDROID_ENV) $1) $(GYP) -f make-android -I build/android-$1/config.gypi \
	  --generator-output=build/android-$1 $$<

android-test-lib-$1: build/android-$1/Makefile
	$$(shell $(ANDROID_ENV) $1) $(MAKE) -j$(JOBS) -C build/android-$1 test 

android-lib-$1: build/android-$1/Makefile
	$$(shell $(ANDROID_ENV) $1) $(MAKE) -j$(JOBS) -C build/android-$1 all
	cd platform/android && ./gradlew --parallel --max-workers=$(JOBS) assemble$(BUILDTYPE)

test-android-core-$1: android-test-lib-$1
	ifndef ANDROID_DX
		$$(error "dx not found. Install android build tools (sdk) and make sure it's on the path")
	
	# Compile main sources and extract the classes (using the test app to get all transitive dependencies in one place)
	cd platform/android && ./gradlew assembleDebug
	unzip -o platform/android/MapboxGLAndroidSDKTestApp/build/outputs/apk/MapboxGLAndroidSDKTestApp-debug.apk classes.dex -d build/android-$1
	
	#Compile Test runner
	javac -sourcepath test/src -d build/android-$1 -source 1.7 -target 1.7 test/src/Main.java
	#Combine and dex
	cd build/android-$1 && dx --dex --output=test.jar *.class classes.dex

	#Ensure clean state on the device
	adb shell "rm -Rf $(ANDROID_LOCAL_WORK_DIR)"
	adb shell "mkdir -p $(ANDROID_LOCAL_WORK_DIR)/test"

	#Push all needed files to the device
	adb push build/android-$1/test.jar $(ANDROID_LOCAL_WORK_DIR)
	adb push test/fixtures $(ANDROID_LOCAL_WORK_DIR)/test
	adb push build/android-$1/$(BUILDTYPE)/lib.target/libmapbox-gl.so $(ANDROID_LOCAL_WORK_DIR)
	adb push build/android-$1/$(BUILDTYPE)/lib.target/libtest-jni-lib.so $(ANDROID_LOCAL_WORK_DIR)

	#Kick off the tests
	adb shell "export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:/system/lib:$(ANDROID_LOCAL_WORK_DIR) && cd $(ANDROID_LOCAL_WORK_DIR) && dalvikvm32 -cp $(ANDROID_LOCAL_WORK_DIR)/test.jar Main"	

apackage: android-lib-$1i

endef

$(foreach abi,$(ANDROID_ABIS),$(eval $(call ANDROID_RULES,$(abi))))

android: android-arm-v7

test-android: test-android-core-arm-v7
	cd platform/android && ./gradlew testReleaseUnitTest --continue

apackage:
	cd platform/android && ./gradlew --parallel-threads=$(JOBS) assemble$(BUILDTYPE)


#### Node targets #####################################################

NODE_PRE_GYP = $(shell npm bin)/node-pre-gyp
NODE_OUTPUT_PATH = build/node-$(BUILD_PLATFORM)-$(BUILD_PLATFORM_VERSION)

ifeq ($(BUILDTYPE), Debug)
	NODE_DEBUG = "--debug"
endif

$(NODE_OUTPUT_PATH)/config.gypi: platform/$(BUILD_PLATFORM)/scripts/configure.sh $(CONFIG_DEPENDENCIES)
	./configure $< $@ $(BUILD_PLATFORM) $(BUILD_PLATFORM_VERSION)

node: $(NODE_OUTPUT_PATH)/config.gypi node_modules $(GYP_DEPENDENCIES)
	$(NODE_PRE_GYP) configure --clang $(NODE_DEBUG) -- -I$< \
	  -Dcoverage= -Dlibuv_cflags= -Dlibuv_ldflags= -Dlibuv_static_libs=
	$(NODE_PRE_GYP) build --clang

xnode: $(NODE_OUTPUT_PATH)/config.gypi $(GYP_DEPENDENCIES)
	$(NODE_PRE_GYP) configure --clang -- -I$< \
	  -Dcoverage= -Dlibuv_cflags= -Dlibuv_ldflags= -Dlibuv_static_libs= \
	  -f xcode
	./platform/node/scripts/create_node_scheme.sh "node test" "`npm bin tape`/tape platform/node/test/js/**/*.test.js"
	./platform/node/scripts/create_node_scheme.sh "npm run test-suite" "platform/node/test/render.test.js"
	open ./build/binding.xcodeproj

.PHONY: test-node
test-node: node
	npm test
	npm run test-suite

#### Qt targets #####################################################

QT_OUTPUT_PATH = build/qt-$(BUILD_PLATFORM)-$(BUILD_PLATFORM_VERSION)
QT_MAKEFILE = $(QT_OUTPUT_PATH)/Makefile

# Cross compilation support
QT_ENV = $(shell MASON_PLATFORM_VERSION=$(BUILD_PLATFORM_VERSION) ./platform/qt/scripts/toolchain.sh)

$(QT_OUTPUT_PATH)/config.gypi: platform/qt/scripts/configure.sh $(CONFIG_DEPENDENCIES)
	$(QT_ENV) ./configure $< $@ $(BUILD_PLATFORM) $(BUILD_PLATFORM_VERSION)

GYP_FLAVOR = make
ifneq ($(HOST_PLATFORM),$(BUILD_PLATFORM))
  ifeq ($(BUILD_PLATFORM), linux)
    GYP_FLAVOR = make-linux
  else ifeq ($(BUILD_PLATFORM), macos)
    GYP_FLAVOR = make-mac
  endif
endif

$(QT_MAKEFILE): platform/qt/platform.gyp $(QT_OUTPUT_PATH)/config.gypi $(GYP_DEPENDENCIES)
	$(QT_ENV) $(GYP) -f $(GYP_FLAVOR) -I $(QT_OUTPUT_PATH)/config.gypi \
	  --generator-output=$(QT_OUTPUT_PATH) $<

qt-lib: $(QT_MAKEFILE)
	$(QT_ENV) $(MAKE) -j$(JOBS) -C $(QT_OUTPUT_PATH) qt-lib

qt-app: $(QT_MAKEFILE)
	$(QT_ENV) $(MAKE) -j$(JOBS) -C $(QT_OUTPUT_PATH) qt-app

qt-qml-app: $(QT_MAKEFILE)
	$(QT_ENV) $(MAKE) -j$(JOBS) -C $(QT_OUTPUT_PATH) qt-qml-app

test-qt-%: $(QT_MAKEFILE) node_modules
	$(QT_ENV) $(MAKE) -j$(JOBS) -C $(QT_OUTPUT_PATH) test
	$(GDB) $(QT_OUTPUT_PATH)/$(BUILDTYPE)/test --gtest_catch_exceptions=0 --gtest_filter=$*

test-qt: test-qt-*

run-qt-app: qt-app
	cd $(QT_OUTPUT_PATH)/$(BUILDTYPE) && ./qmapboxgl

run-qt-qml-app: qt-qml-app
	cd $(QT_OUTPUT_PATH)/$(BUILDTYPE) && ./qquickmapboxgl

test-valgrind-qt-%: $(QT_MAKEFILE) node_modules
	$(QT_ENV) $(MAKE) -j$(JOBS) -C $(QT_OUTPUT_PATH) test
	.mason/mason install valgrind latest
	./scripts/valgrind.sh $(QT_OUTPUT_PATH)/$(BUILDTYPE)/test --gtest_catch_exceptions=0 --gtest_filter=$*

test-valgrind-qt: test-valgrind-qt-*

run-valgrind-qt-app: qt-app
	.mason/mason install valgrind latest
	./scripts/valgrind.sh $(QT_OUTPUT_PATH)/$(BUILDTYPE)/qmapboxgl --test -platform offscreen

#### Linux targets #####################################################

LINUX_OUTPUT_PATH = build/linux-$(BUILD_PLATFORM_VERSION)
LINUX_MAKEFILE = $(LINUX_OUTPUT_PATH)/Makefile

$(LINUX_OUTPUT_PATH)/config.gypi: platform/linux/scripts/configure.sh $(CONFIG_DEPENDENCIES)
	./configure $< $@ linux $(BUILD_PLATFORM_VERSION)

$(LINUX_MAKEFILE): platform/linux/platform.gyp $(LINUX_OUTPUT_PATH)/config.gypi $(GYP_DEPENDENCIES)
	$(GYP) -f make -I $(LINUX_OUTPUT_PATH)/config.gypi \
	  --generator-output=$(LINUX_OUTPUT_PATH) $<

linux: glfw-app render offline

test-linux: node_modules test-*

render: $(LINUX_MAKEFILE)
	$(MAKE) -j$(JOBS) -C $(LINUX_OUTPUT_PATH) mbgl-render

offline: $(LINUX_MAKEFILE)
	$(MAKE) -j$(JOBS) -C $(LINUX_OUTPUT_PATH) mbgl-offline

glfw-app: $(LINUX_MAKEFILE)
	$(MAKE) -j$(JOBS) -C $(LINUX_OUTPUT_PATH) glfw-app

test: $(LINUX_MAKEFILE)
	$(MAKE) -j$(JOBS) -C $(LINUX_OUTPUT_PATH) test

run-glfw-app: glfw-app
	cd $(LINUX_OUTPUT_PATH)/$(BUILDTYPE) && ./mapbox-glfw

ifneq (,$(shell which gdb))
  GDB = gdb -batch -return-child-result -ex 'set print thread-events off' -ex 'run' -ex 'thread apply all bt' --args
endif

test-%: test
	$(GDB) $(LINUX_OUTPUT_PATH)/$(BUILDTYPE)/test --gtest_catch_exceptions=0 --gtest_filter=$*

coverage: test
	scripts/collect-coverage.sh $(LINUX_OUTPUT_PATH)/$(BUILDTYPE)

# Generates a compilation database with ninja for use in clang tooling
compdb: node_modules compdb-$(BUILD_PLATFORM)

compdb-linux: platform/linux/platform.gyp $(LINUX_OUTPUT_PATH)/config.gypi
	$(GYP) -f ninja -I $(LINUX_OUTPUT_PATH)/config.gypi \
	  --generator-output=$(LINUX_OUTPUT_PATH) $<
	deps/ninja/ninja-linux -C $(LINUX_OUTPUT_PATH)/$(BUILDTYPE) \
		-t compdb cc cc_s cxx objc objcxx > $(LINUX_OUTPUT_PATH)/$(BUILDTYPE)/compile_commands.json

compdb-macos: platform/macos/platform.gyp $(MACOS_OUTPUT_PATH)/config.gypi
	$(GYP) -f ninja -I $(MACOS_OUTPUT_PATH)/config.gypi \
	  --generator-output=$(MACOS_OUTPUT_PATH) $<
	deps/ninja/ninja-macos -C $(MACOS_OUTPUT_PATH)/$(BUILDTYPE) \
		-t compdb cc cc_s cxx objc objcxx > $(MACOS_OUTPUT_PATH)/$(BUILDTYPE)/compile_commands.json

tidy: compdb tidy-$(BUILD_PLATFORM)

clang-tools-linux:
	if test -z $(CLANG_TIDY); then .mason/mason install clang-tidy 3.8.0; fi
	if test -z $(CLANG_FORMAT); then .mason/mason install clang-format 3.8.0; fi
	deps/ninja/ninja-linux -C $(LINUX_OUTPUT_PATH)/$(BUILDTYPE) headers

tidy-linux: clang-tools-linux
	scripts/clang-tools.sh $(LINUX_OUTPUT_PATH)/$(BUILDTYPE)

clang-tools-macos:
	if test -z $(CLANG_TIDY); then .mason/mason install clang-tidy 3.8.0; fi
	if test -z $(CLANG_FORMAT); then .mason/mason install clang-format 3.8.0; fi
	deps/ninja/ninja-macos -C $(MACOS_OUTPUT_PATH)/$(BUILDTYPE) headers

tidy-macos: clang-tools-macos
	scripts/clang-tools.sh $(MACOS_OUTPUT_PATH)/$(BUILDTYPE)

check: compdb check-$(BUILD_PLATFORM)

check-linux: clang-tools-linux
	scripts/clang-tools.sh $(LINUX_OUTPUT_PATH)/$(BUILDTYPE) --diff

check-macos: clang-tools-macos
	scripts/clang-tools.sh $(MACOS_OUTPUT_PATH)/$(BUILDTYPE) --diff

#### Miscellaneous targets #####################################################

style-code:
	node scripts/generate-style-code.js

clean:
	-find ./deps/gyp -name "*.pyc" -exec rm {} \;
	-rm -rf ./build \
	        ./platform/android/MapboxGLAndroidSDK/build \
	        ./platform/android/MapboxGLAndroidSDKTestApp/build \
	        ./platform/android/MapboxGLAndroidSDK/src/main/jniLibs \
	        ./platform/android/MapboxGLAndroidSDKTestApp/src/main/jniLibs \
	        ./platform/android/MapboxGLAndroidSDK/src/main/assets

distclean: clean
	-rm -rf ./mason_packages
	-rm -rf ./node_modules
