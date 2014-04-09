all: llmr

-include config.mk

BUILDTYPE ?= Release
PYTHON ?= python
V ?= 1

deps: glsl-optimizer

llmr: deps config.gypi src llmr.gyp
	deps/run_gyp llmr.gyp -Icommon.gypi -Ilinux/platform.gypi -Goutput_dir=./out/ --depth=. --generator-output=./out/ -f make
	make -j8 -C out BUILDTYPE=Release V=$(V) llmr

gtest: deps config.gypi src llmr.gyp
	deps/run_gyp llmr.gyp -Icommon.gypi -Goutput_dir=./out/ --depth=. --generator-output=./build/gtest -f make
	make -C build/gtest gtest V=$(V)

lproj: deps config.gypi src linux/llmr-app.gyp
	deps/run_gyp linux/llmr-app.gyp -Icommon.gypi -Imacosx/platform.gypi -Goutput_dir=./out/ --depth=. --generator-output=./ -f xcode
	open ./linux/llmr-app.xcodeproj

# build OS X app with Xcode
xproj: deps config.gypi src macosx/llmr-app.gyp
	deps/run_gyp macosx/llmr-app.gyp -Icommon.gypi -Imacosx/platform.gypi -Goutput_dir=./out/ --depth=. --generator-output=./ -f xcode
	open ./macosx/llmr-app.xcodeproj

# build iOS app with Xcode
iproj: deps config.gypi src ios/llmr-app.gyp
	deps/run_gyp ios/llmr-app.gyp -Icommon.gypi -Iios/platform.gypi -Goutput_dir=./out/ --depth=. --generator-output=./ -f xcode
	open ./ios/llmr-app.xcodeproj

test: deps config.gypi src test/test.gyp
	deps/run_gyp test/test.gyp -Icommon.gypi -Goutput_dir=. --depth=. --generator-output=./build/test -f make
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
