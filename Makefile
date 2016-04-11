export BUILDTYPE ?= Release
export ENABLE_COVERAGE ?= 0

ifeq ($(shell uname -s), Darwin)
  export JOBS ?= $(shell sysctl -n hw.ncpu)
else ifeq ($(shell uname -s), Linux)
  export JOBS ?= $(shell grep --count processor /proc/cpuinfo)
else
  $(error Cannot determine host platform)
endif

RUN = +$(MAKE) -f scripts/main.mk

default:
	@printf "You must specify a valid target\n"

#### OS X targets ##############################################################

OSX_PROJ_PATH = build/osx-x86_64/platform/osx/platform.xcodeproj

osx:
	$(RUN) PLATFORM=osx Xcode/All

$(OSX_PROJ_PATH): platform/osx/platform.gyp platform/osx/scripts/configure.sh mbgl.gypi test/test.gypi
	$(RUN) PLATFORM=osx Xcode/__project__

xproj: $(OSX_PROJ_PATH)
	open $(OSX_PROJ_PATH)

$(OSX_PROJ_PATH)/xcshareddata/xcschemes/osxtest.xcscheme: platform/osx/scripts/osxtest.xcscheme
	mkdir -p $(basename $@)
	cp $< $@

test-osx: $(OSX_PROJ_PATH) $(OSX_PROJ_PATH)/xcshareddata/xcschemes/osxtest.xcscheme node_modules/express
	set -o pipefail && xcodebuild -project $(OSX_PROJ_PATH) -configuration $(BUILDTYPE) -target test build | xcpretty
	build/osx-x86_64/$(BUILDTYPE)/test || ([[ $$? == 139 || $$? == 134 ]] && cat `ls -t1 ~/Library/Logs/DiagnosticReports/* | head -n1`; exit 1)
	set -o pipefail && xcodebuild -project $(OSX_PROJ_PATH) -configuration $(BUILDTYPE) -scheme osxtest test | xcpretty

#### iOS targets ##############################################################

IOS_PROJ_PATH = build/ios-all/platform/ios/platform.xcodeproj

ios:
	$(RUN) PLATFORM=ios Xcode/All

$(IOS_PROJ_PATH): platform/ios/platform.gyp platform/ios/scripts/configure.sh mbgl.gypi test/test.gypi
	$(RUN) PLATFORM=ios Xcode/__project__

iproj: $(IOS_PROJ_PATH)
	open $(IOS_PROJ_PATH)

test-ios: $(IOS_PROJ_PATH)
	set -o pipefail && xcodebuild -project $(IOS_PROJ_PATH) -configuration $(BUILDTYPE) \
	  -sdk iphonesimulator -destination 'platform=iOS Simulator,name=iPhone 6,OS=latest' \
	  -target test build | xcpretty
	ios-sim start
	ios-sim launch build/ios-all/$(BUILDTYPE)-iphonesimulator/ios-test.app --verbose

ipackage: $(IOS_PROJ_PATH)
	BITCODE=$(BITCODE) FORMAT=$(FORMAT) BUILD_DEVICE=$(BUILD_DEVICE) SYMBOLS=$(SYMBOLS) \
	BUNDLE_RESOURCES=YES PLACE_RESOURCE_BUNDLES_OUTSIDE_FRAMEWORK=YES \
	./platform/ios/scripts/package.sh

ipackage-strip: $(IOS_PROJ_PATH)
	BITCODE=$(BITCODE) FORMAT=$(FORMAT) BUILD_DEVICE=$(BUILD_DEVICE) SYMBOLS=NO \
	BUNDLE_RESOURCES=YES PLACE_RESOURCE_BUNDLES_OUTSIDE_FRAMEWORK=YES \
	./platform/ios/scripts/package.sh

ipackage-sim: $(IOS_PROJ_PATH)
	BUILDTYPE=Debug BITCODE=$(BITCODE) FORMAT=dynamic BUILD_DEVICE=false SYMBOLS=$(SYMBOLS) \
	BUNDLE_RESOURCES=YES PLACE_RESOURCE_BUNDLES_OUTSIDE_FRAMEWORK=YES \
	./platform/ios/scripts/package.sh

iframework: $(IOS_PROJ_PATH)
	BITCODE=$(BITCODE) FORMAT=dynamic BUILD_DEVICE=$(BUILD_DEVICE) SYMBOLS=$(SYMBOLS) \
	./platform/ios/scripts/package.sh

ifabric: $(IOS_PROJ_PATH)
	BITCODE=$(BITCODE) FORMAT=$(FORMAT) BUILD_DEVICE=$(BUILD_DEVICE) SYMBOLS=NO BUNDLE_RESOURCES=YES \
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
	-find ./build -type f -not -path '*/*.xcodeproj/*' -exec rm {} \;
	-rm -rf ./platform/android/MapboxGLAndroidSDK/build \
	        ./platform/android/MapboxGLAndroidSDKTestApp/build \
	        ./platform/android/MapboxGLAndroidSDK/src/main/jniLibs \
	        ./platform/android/MapboxGLAndroidSDKTestApp/src/main/jniLibs \
	        ./platform/android/MapboxGLAndroidSDK/src/main/assets

distclean: clean
	-rm -rf ./mason_packages
