# Building & Developing Mapbox GL Native from Source

**Just trying to use Mapbox GL Native? You don't need to read this stuff! We
provide [easy-to-install, prebuilt versions of the Mapbox SDKs for iOS and Android
that you can download instantly and get started with fast](https://www.mapbox.com/mobile/).**

Still with us? These are the instructions you'll need to build Mapbox GL Native
from source on a variety of platforms and set up a development environment.

Your journey will start with getting the source code, then installing the
dependencies, and then setting up a development environment, which varies
depending on your operating system and what platform you want to develop for.

## 1: Getting the Source

Mapbox GL Native uses git submodules, so you'll need to initialize and update
them after cloning the source from Git.

    git clone https://github.com/mapbox/mapbox-gl-native.git
    cd mapbox-gl-native
    git submodule init
    git submodule update

Default styles in the [Mapbox GL Style Spec format](https://github.com/mapbox/mapbox-gl-style-spec) are included at `./styles` as a submodule. See the [styles project](https://github.com/mapbox/mapbox-gl-styles) to report problems or to contribute to these styles.

## 2: Installing Dependencies

These dependencies are required for all operating systems and all platform
targets.

 - Modern C++ compiler that supports `-std=c++14`
   - clang++ 3.5 or later _or_
   - g++-4.9 or later
 - [Boost headers](http://boost.org/)
 - [`zlib`](http://www.zlib.net/)
 - [`libpng`](http://www.libpng.org/pub/png/libpng.html)
 - [`libuv 0.10+`](https://github.com/joyent/libuv)
 - [`glfw 3.1+`](http://www.glfw.org/)
 - `pkg-config` (for build only)
 - Python 2.x (for build only)
 - [Imagemagick](http://www.imagemagick.org/)

Depending on your operating system and target, you'll need additional
dependencies:

### Additional Dependencies for Linux

 - [`libcurl`](http://curl.haxx.se/libcurl/) (depends on OpenSSL)

### Additional Dependencies for OS X

 - Apple Command Line Tools (available at [Apple Developer](https://developer.apple.com/downloads))
 - [Homebrew](http://brew.sh)
 - [xcpretty](https://github.com/supermarin/xcpretty)
 - [Cask](http://caskroom.io/) (if building for Android)

## 3: Setting up a development environment & building

This depends on what operating systems you're using, and what platforms you're
developing for:

**on OS X**

* [For OS X on OS X](docs/DEVELOP_OSX.md)
* [For iOS on OS X](docs/DEVELOP_IOS_OSX.md)
* [For Android on OS X](docs/DEVELOP_ANDROID_OSX.md)

**on Linux**

* [For Linux on Linux](docs/DEVELOP_LINUX.md)
* [For Android on Linux](docs/DEVELOP_ANDROID_LINUX.md)

**Node.js bindings**

* [Developing Node.js bindings](docs/DEVELOP_NODE.md)
