# Building & Developing Mapbox GL Native from Source

**Just trying to use Mapbox GL Native? You don't need to read this stuff! We
provide [easy-to-install, prebuilt versions of the Mapbox Maps SDKs for iOS and Android
that you can download instantly and get started with fast](https://www.mapbox.com/install/).**

Still with us? These are the instructions you'll need to build Mapbox GL Native
from source on a variety of platforms and set up a development environment.

Your journey will start with getting the source code, then installing the
dependencies, and then setting up a development environment, which varies
depending on your operating system and what platform you want to develop for.

## 1: Getting the source

Clone the git repository:

    git clone https://github.com/mapbox/mapbox-gl-native.git
    cd mapbox-gl-native

## 2: Installing dependencies

These dependencies are required for all operating systems and all platform
targets.

 - Modern C++ compiler that supports `-std=c++14`\*
   - clang++ 3.5 or later _or_
   - g++-4.9 or later
 - [CMake](https://cmake.org/) 3.1 or later (for build only)
 - [cURL](https://curl.haxx.se) (for build only)
 - [Node.js](https://nodejs.org/) 4.2.1 or later (for build only)
 - [`pkg-config`](https://wiki.freedesktop.org/www/Software/pkg-config/) (for build only)

**Note**: We partially support C++14 because GCC 4.9 does not fully implement the
final draft of the C++14 standard. More information in [DEVELOPING.md](DEVELOPING.md).

Depending on your operating system and target, you'll need additional
dependencies:

### Additional dependencies for Linux

 - [`libcurl`](http://curl.haxx.se/libcurl/) (depends on OpenSSL)

### Additional dependencies for macOS

 - Apple Command Line Tools (available at [Apple Developer](https://developer.apple.com/download/more/))
 - [Homebrew](http://brew.sh)
 - [Cask](http://caskroom.io/) (if building for Android)
 - [xcpretty](https://github.com/supermarin/xcpretty) (`gem install xcpretty`)

### Optional dependencies

- [ccache](https://ccache.samba.org) (for build only; improves recompilation performance)

## 3: Setting up a development environment & building

See the relevant SDK documentation for next steps:

* [Maps SDK for Android](platform/android/)
* [Maps SDK for iOS](platform/ios/)
* [Maps SDK for iOS](platform/macos/)
* [Mapbox Qt SDK](platform/qt/)
* [Mapbox GL Native on Linux](platform/linux/)
* [node-mapbox-gl-native](platform/node/)

## 4: Keeping up to date

This repository uses Git submodules, which should be automatically checked out when you first run a `make` command for one of the above platforms. These submodules are not updated automatically and we recommended that you run `git submodule update` after pulling down new commits to this repository.
