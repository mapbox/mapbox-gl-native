-include config.mk

BUILDTYPE ?= Release
PYTHON ?= python
V ?= 1

all: llmr

llmr: config.gypi src llmr.gyp
	$(MAKE) -C out BUILDTYPE=Release V=$(V) llmr-osx

# build OS X app with pure make
app: config.gypi src macosx/llmr-app.gyp
	deps/run_gyp macosx/llmr-app.gyp -Goutput_dir=./out/ --depth=. --generator-output=./build/macosx-make -f make
	make -C build/macosx-make V=$(V)
	open build/macosx-make/out/Release/llmr.app

xcode: config.gypi llmr.gyp
	deps/run_gyp llmr.gyp -Goutput_dir=./out/ --depth=. --generator-output=./ -f xcode

# build OS X with xcodebuild
xapp: xcode config.gypi src macosx/llmr-app.gyp
	deps/run_gyp macosx/llmr-app.gyp -Goutput_dir=./out/ --depth=. --generator-output=./ -f xcode
	xcodebuild -project ./macosx/llmr-app.xcodeproj
	open macosx/build/Release/llmr.app

clean:
	-rm -rf out
	-rm -rf build
	-rm -rf macosx/build

distclean:
	-rm -f config.gypi
	-rm -f config.mk
	-rm -rf llmr.xcodeproj

test: all
	echo test

.PHONY: test
