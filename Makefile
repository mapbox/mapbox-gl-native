-include config.mk

BUILDTYPE ?= Release
PYTHON ?= python
V ?= 1

all: llmr

deps: glsl-optimizer

llmr: deps config.gypi src llmr.gyp
	@if [ ! -f out ]; then echo 'please run ./configure first'; else $(MAKE) -C out BUILDTYPE=Release V=$(V) llmr-x86; fi;

gtest: deps config.gypi src llmr.gyp
	deps/run_gyp llmr.gyp -Goutput_dir=./out/ --depth=. --generator-output=./build/gtest -f make
	make -C build/gtest gtest V=$(V)

# build OS X app with pure make
app: deps config.gypi src macosx/llmr-app.gyp
	deps/run_gyp macosx/llmr-app.gyp -Goutput_dir=./out/ --depth=. --generator-output=./build/macosx-make -f make
	make -C build/macosx-make V=$(V)
	open build/macosx-make/out/Release/llmr.app

linux: deps config.gypi src linux/llmr-app.gyp
	deps/run_gyp linux/llmr-app.gyp -Goutput_dir=./out/ --depth=. --generator-output=./build/linux-make -f make
	make -C build/linux-make V=$(V)
	./build/linux-make/out/Release/llmr.app


# build OS X app with Xcode
lproj: deps config.gypi src linux/llmr-app.gyp
	deps/run_gyp linux/llmr-app.gyp -Goutput_dir=./out/ --depth=. --generator-output=./ -f xcode
	open ./linux/llmr-app.xcodeproj

# build just xcode project for libllmr
xcode: deps config.gypi llmr.gyp
	deps/run_gyp llmr.gyp -Goutput_dir=./out/ --depth=. --generator-output=./ -f xcode

# build OS X app with Xcode
xproj: deps config.gypi src macosx/llmr-app.gyp
	deps/run_gyp macosx/llmr-app.gyp -Goutput_dir=./out/ --depth=. --generator-output=./ -f xcode
	open ./macosx/llmr-app.xcodeproj

# build OS X app with xcodebuild
xapp: deps config.gypi src macosx/llmr-app.gyp
	deps/run_gyp macosx/llmr-app.gyp -Goutput_dir=./out/ --depth=. --generator-output=./ -f xcode
	xcodebuild -project ./macosx/llmr-app.xcodeproj
	open macosx/build/Release/llmr.app

# build iOS app with Xcode
iproj: deps config.gypi src ios/llmr-app.gyp
	deps/run_gyp ios/llmr-app.gyp -Goutput_dir=./out/ --depth=. --generator-output=./ -f xcode
	open ./ios/llmr-app.xcodeproj

# build iOS app with xcodebuild
iapp: deps config.gypi src ios/llmr-app.gyp
	deps/run_gyp ios/llmr-app.gyp -Goutput_dir=./out/ --depth=. --generator-output=./ -f xcode
	xcodebuild -project ./ios/llmr-app.xcodeproj
	# launch app with ios-sim?

isim: deps config.gypi src ios/llmr-app.gyp
	deps/run_gyp ios/llmr-app.gyp -Goutput_dir=./out/ --depth=. --generator-output=./ -f xcode
	xcodebuild -project ./ios/llmr-app.xcodeproj -arch i386 -sdk iphonesimulator
	# does not work
	#"/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/Applications/iPhone Simulator.app/Contents/MacOS/iPhone Simulator" -SimulateApplication ios/build/Release-iphonesimulator/llmr.app/llmr

test: deps config.gypi src test/test.gyp
	deps/run_gyp test/test.gyp -Goutput_dir=. --depth=. --generator-output=./build/test -f make
	make -C build/test V=$(V)
	@for FILE in build/test/Release/test_*; do \
		$${FILE}; \
	done

glsl-optimizer: bin/node_modules/glsl-optimizer/build/Release/glslOptimizer.node

bin/node_modules/glsl-optimizer/build/Release/glslOptimizer.node:
	cd bin && npm install

clean:
	-rm -rf out
	-rm -rf build
	-rm -rf macosx/build
	-rm -rf ios/build

distclean:
	-rm -f config.gypi
	-rm -f config.mk
	-rm -rf llmr.xcodeproj
	-rm -rf macosx/llmr-app.xcodeproj
	-rm -rf ios/llmr-app.xcodeproj

.PHONY: test linux
