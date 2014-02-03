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

## Ubuntu

Install a `-std=c++11` capable compiler

    sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
    sudo apt-get install gcc-4.8 g++-4.8

Install boost (we only need headers):

    sudo apt-get install libboost-dev

Install glfw3 dependencies:

    sudo apt-get install cmake libxi-dev libglu1-mesa-dev x11proto-randr-dev x11proto-xext-dev libxrandr-dev x11proto-xf86vidmode-dev libxxf86vm-dev libglu1-mesa-dev freeglut3-dev mesa-common-dev

Install glfw3:

    git clone https://github.com/glfw/glfw.git
    cd glfw
    mkdir build
    cd build
    cmake ../
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
