An OpenGL renderer for [Mapbox vector tiles](https://www.mapbox.com/blog/vector-tiles),
implemented in C++, targeting iOS & OS X.

# Build instructions

## OS X

Install boost and [glfw3](http://www.glfw.org/docs/latest/):

```
brew install boost
brew install homebrew/versions/glfw3
```

Then configure the project:

    ./configure

Options include:

 - --boost=/usr/local
 - --glfw3=/usr/local (by default pkg-config will be used)

Then you can build the OS X app with make:

    make app

Or generate a dual iOS/OS X-compatible Xcode project for `libllmr` to include as a subproject:

    make xcode # then open llmr.xcodeproj

## iOS

For iOS you'll likely need `armv7` and `armv7s` libs for targeting devices and `i386` libs for the simulator. The best way to handle this is to build for all those architectures and then combine the libs into a single universal lib. We'll do this with the scripts from [mapnik-packaging](https://github.com/mapnik/mapnik-packaging.git)

From within the `llmr-native` directory:

    git clone --depth=0 https://github.com/mapnik/mapnik-packaging.git
    cd mapnik-packaging/osx/
    export CXX11=true
    (source iPhoneSimulator.sh; ./scripts/build_png.sh; \
      source MacOSX.sh; ./scripts/build_png.sh; \
      source iPhoneOS.sh; ./scripts/build_png.sh; \
      source iPhoneOSs.sh;  ./scripts/build_png.sh; \
      ./scripts/make_universal.sh)
    export UNIVERSAL_LIBS=`pwd`/out/build-cpp11-libcpp-universal/
    export PNG_INCLUDES=`pwd`/out/build-cpp11-libcpp-i386/
    cd ../../
    ./configure --png-libpath=${UNIVERSAL_LIBS} --png-includes=${PNG_INCLUDES}



## Ubuntu

Ensure you have git and other build essentials:

    sudo apt-get install git cmake make pkg-config

Install a `-std=c++11` capable compiler

    sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
    sudo apt-get update
    sudo apt-get install gcc-4.8 g++-4.8

Install boost (we only need headers):

    sudo apt-get install libboost-dev

Install libpng-dev:

    sudo apt-get install libpng-dev

Install glfw3 dependencies:

    sudo apt-get install libxi-dev libglu1-mesa-dev x11proto-randr-dev x11proto-xext-dev libxrandr-dev x11proto-xf86vidmode-dev libxxf86vm-dev

Install glfw3:

    git clone https://github.com/glfw/glfw.git
    cd glfw
    mkdir build
    cd build
    cmake ../ -DCMAKE_C_COMPILER_ENV_VAR=gcc-4.8
    make
    sudo make install
    cd ../../

Build `libllmr`:

    git clone git@github.com:mapbox/llmr-native.git
    cd llmr-native
    export CXX="g++-4.8"
    ./configure
    make

Note: build will not compile until https://github.com/mapbox/llmr-native/issues/26 is fixed.

# Style protobuf

We're encoding the styling information as a protocol buffer, according to
proto/style.proto. The reason for doing so is that we don't have to ship with a
JSON/yaml parser. To test the conversion script, run

```
bin/convert-style.js | protoc --decode=style proto/style.proto
```

You should see a nicely formatted hierarchical output of the converted style.


To update the included default style, use

```
bin/convert-style.js > resources/style.pbf && bin/build-style.js
```


# Usage

- Press 'R' to reset the transform
- Press 'N' to reset north
- Press Tab to toggle debug information
- Press Esc to quit
