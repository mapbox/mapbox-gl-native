-include config.mk

BUILDTYPE ?= Release
PYTHON ?= python
V ?= 1

all: llmr

llmr: config.gypi src llmr.gyp
	$(MAKE) -C out BUILDTYPE=Release V=$(V) llmr-x86

# build OS X app with pure make
app: config.gypi src macosx/llmr-app.gyp
	deps/run_gyp macosx/llmr-app.gyp -Goutput_dir=./out/ --depth=. --generator-output=./build/macosx-make -f make
	make -C build/macosx-make V=$(V)
	open build/macosx-make/out/Release/llmr.app

linux: config.gypi src linux/llmr-app.gyp
	deps/run_gyp linux/llmr-app.gyp -Goutput_dir=./out/ --depth=. --generator-output=./build/linux-make -f make
	make -C build/linux-make V=$(V)
	./build/linux-make/out/Release/llmr.app

# build just xcode project for libllmr
xcode: config.gypi llmr.gyp
	deps/run_gyp llmr.gyp -Goutput_dir=./out/ --depth=. --generator-output=./ -f xcode

# build OS X app with Xcode
xproj: config.gypi src macosx/llmr-app.gyp
	deps/run_gyp macosx/llmr-app.gyp -Goutput_dir=./out/ --depth=. --generator-output=./ -f xcode
	open ./macosx/llmr-app.xcodeproj

# build OS X app with xcodebuild
xapp: config.gypi src macosx/llmr-app.gyp
	deps/run_gyp macosx/llmr-app.gyp -Goutput_dir=./out/ --depth=. --generator-output=./ -f xcode
	xcodebuild -project ./macosx/llmr-app.xcodeproj
	open macosx/build/Release/llmr.app

# build iOS app with Xcode
iproj: config.gypi src ios/llmr-app.gyp
	deps/run_gyp ios/llmr-app.gyp -Goutput_dir=./out/ --depth=. --generator-output=./ -f xcode
	open ./ios/llmr-app.xcodeproj

# build iOS app with xcodebuild
iapp: config.gypi src ios/llmr-app.gyp
	deps/run_gyp ios/llmr-app.gyp -Goutput_dir=./out/ --depth=. --generator-output=./ -f xcode
	xcodebuild -project ./ios/llmr-app.xcodeproj
	# launch app with ios-sim?

isim: config.gypi src ios/llmr-app.gyp
	deps/run_gyp ios/llmr-app.gyp -Goutput_dir=./out/ --depth=. --generator-output=./ -f xcode
	xcodebuild -project ./ios/llmr-app.xcodeproj -arch i386 -sdk iphonesimulator
	# does not work
	#"/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/Applications/iPhone Simulator.app/Contents/MacOS/iPhone Simulator" -SimulateApplication ios/build/Release-iphonesimulator/llmr.app/llmr

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

test: all
	echo test

.PHONY: test linux
