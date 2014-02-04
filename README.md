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

See all the options by calling `./configure --help`

```sh
$ ./configure --help
Usage: configure [options]

Options:
  -h, --help            show this help message and exit
  --debug               Also build debug build
  --boost=BOOST_ROOT    Path to boost (defaults to /usr/local)
  --glfw3=GLFW3         Path to gflw3 (defaults to using pkg-config)
  --png=PNG             Path to png (defaults to using pkg-config)
  --png-includes=PNG_INCLUDES
                        Path to png includes
  --png-libpath=PNG_LIBPATH
                        Path to png libs
```


Then you can build the OS X app with make:

    make app

Or generate a dual iOS/OS X-compatible Xcode project for `libllmr` to include as a subproject:

    make xcode # then open llmr.xcodeproj

## iOS

Install boost: 

    brew install boost

Because `libpng` isn't included in the iOS SDK, you will need to build a cross-architecture version yourself. Run `./ios/setup_libpng.sh`, which is derived from Mapnik's cross-architecture build scripts. This will also run `./configure`. 

Then you can build the iOS app with make: 

    make iapp

Consider `sudo npm install -g ios-sim` for auto-launching the simulator, but it can be tricky and it's better to run on an ARM-based device anyway. To do this, open `./ios/llmr-app.xcodeproj` in Xcode, then build and run on the simulator or a device yourself. 

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

## Desktop

- Press 'R' to reset the transform
- Press 'N' to reset north
- Press Tab to toggle debug information
- Press Esc to quit

## Mobile

- Pan to move
- Pinch to zoom
- Use two fingers to rotate
- Double-tap to zoom in one level
- Two-finger single-tap to zoom out one level
- Long-press to reset north
- Two-finger long press to reset the transform
- Three-finger long press to toggle debug information