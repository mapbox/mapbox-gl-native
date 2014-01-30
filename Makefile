-include config.mk

BUILDTYPE ?= Release
PYTHON ?= python
V ?= 1

all: out/Makefile llmr

config.gypi: configure
	$(PYTHON) ./configure

llmr: config.gypi out/Makefile llmr.gyp
	$(MAKE) -C out BUILDTYPE=Release V=$(V)

# build OS X app with pure make
app: config.gypi src macosx/llmr-app.gyp
	deps/run_gyp macosx/llmr-app.gyp -Goutput_dir=./out/ --depth=. -Icommon.gypi --generator-output=./build/macosx-make -f make
	make -C build/macosx-make
	open build/macosx-make/out/Release/llmr.app

# build OS X with xcodebuild
xapp: config.gypi src macosx/llmr-app.gyp
	deps/run_gyp macosx/llmr-app.gyp -Goutput_dir=./out/ --depth=. -Icommon.gypi --generator-output=./ -f xcode
	xcodebuild -project ./macosx/llmr-app.xcodeproj -arch x86_64
	open macosx/build/Release/llmr.app

ios:
	deps/run_gyp llmr.gyp -DOS=ios -Goutput_dir=./out/ --depth=. -Icommon.gypi --generator-output=./ios -f xcode

xcode: config.gypi llmr.gyp config.gypi
	deps/run_gyp macosx/llmr-app.gyp -Goutput_dir=./out/ --depth=. -Icommon.gypi --generator-output=./build/macosx-xcode -f xcode

out/Makefile: common.gypi llmr.gyp config.gypi

clean:
	-rm -rf out/Makefile
	-find out/ -name '*.o' -o -name '*.a' | xargs rm -rf
	-rm -rf build
	-rm -rf macosx/build

distclean:
	-rm -rf out
	-rm -f config.gypi
	-rm -f config.mk
	-rm -rf llmr.xcodeproj

test: all
	echo test

.PHONY: test
