export BUILDTYPE ?= Release
export ENABLE_COVERAGE ?= 0

ifeq ($(shell uname -s), Darwin)
  export JOBS ?= $(shell sysctl -n hw.ncpu)
else ifeq ($(shell uname -s), Linux)
  export JOBS ?= $(shell grep --count processor /proc/cpuinfo)
else
  $(error Cannot determine host platform)
endif

ifeq ($(V), 1)
  export XCPRETTY
else
  export XCPRETTY ?= | xcpretty
endif

ifneq (,$(wildcard .git/.))
.mason:
	git submodule update --init
else
.mason: ;
endif

RUN = +$(MAKE) -f scripts/main.mk

default:
	@printf "You must specify a valid target\n"

# Depend on gyp includes plus directories, so that projects are regenerated when
# files are added or removed.
GYP_DEPENDENCIES = mbgl.gypi test/test.gypi bin/*.gypi $(shell find src include -type d)

#### OS X targets ##############################################################

OSX_OUTPUT_PATH = build/osx
OSX_PROJ_PATH = $(OSX_OUTPUT_PATH)/platform/osx/platform.xcodeproj
OSX_WORK_PATH = platform/osx/osx.xcworkspace
OSX_USER_DATA_PATH = $(OSX_WORK_PATH)/xcuserdata/$(USER).xcuserdatad

$(OSX_OUTPUT_PATH)/config.gypi: platform/osx/scripts/configure.sh .mason configure
	MASON_PLATFORM=osx ./configure $< $@

$(OSX_OUTPUT_PATH)/mbgl.xcconfig: $(OSX_OUTPUT_PATH)/config.gypi
	./scripts/export-xcconfig.py $< $@

$(OSX_PROJ_PATH): platform/osx/platform.gyp $(OSX_OUTPUT_PATH)/config.gypi $(OSX_OUTPUT_PATH)/mbgl.xcconfig $(GYP_DEPENDENCIES)
	./deps/run_gyp -f xcode --depth=. --generator-output=$(OSX_OUTPUT_PATH) $<

osx: $(OSX_PROJ_PATH)
	set -o pipefail && xcodebuild \
	  -derivedDataPath $(OSX_OUTPUT_PATH) \
	  -configuration $(BUILDTYPE) \
	  -workspace $(OSX_WORK_PATH) -scheme CI build $(XCPRETTY)

xproj: $(OSX_PROJ_PATH) $(OSX_WORK_PATH)
	mkdir -p "$(OSX_USER_DATA_PATH)"
	cp platform/osx/WorkspaceSettings.xcsettings "$(OSX_USER_DATA_PATH)/WorkspaceSettings.xcsettings"
	open $(OSX_WORK_PATH)

test-osx: osx node_modules/express
	ulimit -c unlimited && ($(OSX_OUTPUT_PATH)/Build/Products/$(BUILDTYPE)/test & pid=$$! && wait $$pid \
	  || (lldb -c /cores/core.$$pid --batch --one-line 'thread backtrace all' --one-line 'quit' && exit 1))
	set -o pipefail && xcodebuild \
	  -derivedDataPath $(OSX_OUTPUT_PATH) \
	  -configuration $(BUILDTYPE) \
	  -workspace $(OSX_WORK_PATH) -scheme CI test $(XCPRETTY)

genstrings:
	genstrings -u -o platform/osx/sdk/Base.lproj platform/darwin/src/*.{m,mm}
	genstrings -u -o platform/osx/sdk/Base.lproj platform/osx/src/*.{m,mm}
	genstrings -u -o platform/ios/resources/Base.lproj platform/ios/src/*.{m,mm}
	-find platform/ios/resources platform/osx/sdk -path '*/Base.lproj/*.strings' -exec \
		textutil -convert txt -extension strings -inputencoding UTF-16 -encoding UTF-8 {} \;
	mv platform/osx/sdk/Base.lproj/Foundation.strings platform/darwin/resources/Base.lproj/

#### iOS targets ##############################################################

IOS_OUTPUT_PATH = build/ios
IOS_PROJ_PATH = $(IOS_OUTPUT_PATH)/platform/ios/platform.xcodeproj
IOS_WORK_PATH = platform/ios/ios.xcworkspace
IOS_USER_DATA_PATH = $(IOS_WORK_PATH)/xcuserdata/$(USER).xcuserdatad

$(IOS_OUTPUT_PATH)/config.gypi: platform/ios/scripts/configure.sh .mason configure
	MASON_PLATFORM=ios ./configure $< $@

$(IOS_OUTPUT_PATH)/mbgl.xcconfig: $(IOS_OUTPUT_PATH)/config.gypi
	./scripts/export-xcconfig.py $< $@

$(IOS_PROJ_PATH): platform/ios/platform.gyp $(IOS_OUTPUT_PATH)/config.gypi $(IOS_OUTPUT_PATH)/mbgl.xcconfig $(GYP_DEPENDENCIES)
	./deps/run_gyp -f xcode --depth=. --generator-output=$(IOS_OUTPUT_PATH) $<

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

# Builds a particular android architecture.
android-lib-%:
	$(RUN) PLATFORM=android SUBPLATFORM=$* Makefile/all

# Builds the default Android library
.PHONY: android-lib
android-lib: android-lib-arm-v7

# Builds the selected/default Android library
.PHONY: android
android: android-lib
	cd platform/android && ./gradlew --parallel --max-workers=$(JOBS) assemble$(BUILDTYPE)

# Builds all android architectures for distribution.
.PHONY: apackage
apackage: android-lib-arm-v5 android-lib-arm-v7 android-lib-arm-v8 android-lib-x86 android-lib-x86-64 android-lib-mips
	cd platform/android && ./gradlew --parallel-threads=$(JOBS) assemble$(BUILDTYPE)

test-android:
	cd platform/android && ./gradlew testReleaseUnitTest --continue

#### Node targets #####################################################

node_modules: package.json
	npm update # Install dependencies but don't run our own install script.

.PHONY: node
node: node_modules
	$(RUN) PLATFORM=node Makefile/node

.PHONY: xnode
xnode:
	$(RUN) Xcode/node
	./platform/node/scripts/create_node_scheme.sh "node test" "`npm bin tape`/tape platform/node/test/js/**/*.test.js"
	./platform/node/scripts/create_node_scheme.sh "npm run test-suite" "platform/node/test/render.test.js"
	open ./build/binding.xcodeproj

.PHONY: test-node
test-node: node
	npm test
	npm run test-suite

#### Qt targets #####################################################

.PHONY: qt-lib
qt-lib:
	$(RUN) PLATFORM=qt Makefile/qt-lib

.PHONY: qt-app
qt-app:
	$(RUN) PLATFORM=qt Makefile/qt-app

.PHONY: run-qt-app
run-qt-app: qt-app
	$(RUN) PLATFORM=qt run-qt-app

.PHONY: qt-qml-app
qt-qml-app:
	$(RUN) PLATFORM=qt Makefile/qt-qml-app

.PHONY: run-qt-qml-app
run-qt-qml-app: qt-qml-app
	$(RUN) PLATFORM=qt run-qt-qml-app

.PHONY: test-qt
test-qt: node_modules/express
	$(RUN) PLATFORM=qt test-*

#### Miscellaneous targets #####################################################

.PHONY: linux
linux: glfw-app render offline

.PHONY: test-linux
test-linux: test-*

.PHONY: glfw-app
glfw-app:
	$(RUN) Makefile/glfw-app

.PHONY: run-glfw-app
run-glfw-app: glfw-app
	$(RUN) run-glfw-app

.PHONY: run-valgrind-glfw-app
run-valgrind-glfw-app: glfw-app
	$(RUN) run-valgrind-glfw-app

.PHONY: test
test:
	$(RUN) Makefile/test

test-%: node_modules/express
	$(RUN) test-$*

node_modules/express:
	npm install express@4.11.1

.PHONY: check
check:
	$(RUN) BUILDTYPE=Debug ENABLE_COVERAGE=1 check

.PHONY: render
render:
	$(RUN) Makefile/mbgl-render

.PHONY: offline
offline:
	$(RUN) Makefile/mbgl-offline

# Generates a compilation database with ninja for use in clang tooling
.PHONY: compdb
compdb:
	$(RUN) Ninja/compdb

.PHONY: tidy
tidy:
	$(RUN) tidy

clean:
	-find ./deps/gyp -name "*.pyc" -exec rm {} \;
	-rm -rf ./build \
	        ./platform/android/MapboxGLAndroidSDK/build \
	        ./platform/android/MapboxGLAndroidSDKTestApp/build \
	        ./platform/android/MapboxGLAndroidSDK/src/main/jniLibs \
	        ./platform/android/MapboxGLAndroidSDKTestApp/src/main/jniLibs \
	        ./platform/android/MapboxGLAndroidSDK/src/main/assets \
	        ./node_modules

distclean: clean
	-rm -rf ./mason_packages
