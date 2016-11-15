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

Clone the git repository:

    git clone https://github.com/mapbox/mapbox-gl-native.git
    cd mapbox-gl-native

## 2: Installing Dependencies

These dependencies are required for all operating systems and all platform
targets.

 - Modern C++ compiler that supports `-std=c++14`
   - clang++ 3.5 or later _or_
   - g++-5 or later
 - [CMake](https://cmake.org/) 3.1 or later (for build only)
 - [cURL](https://curl.haxx.se) (for build only)
 - [Node.js](https://nodejs.org/) 4.2.1 or later (for build only)
 - [`pkg-config`](https://wiki.freedesktop.org/www/Software/pkg-config/) (for build only)

Depending on your operating system and target, you'll need additional
dependencies:

### Additional Dependencies for Linux

 - [`libcurl`](http://curl.haxx.se/libcurl/) (depends on OpenSSL)

### Additional Dependencies for macOS

 - Apple Command Line Tools (available at [Apple Developer](https://developer.apple.com/download/more/))
 - [Homebrew](http://brew.sh)
 - [Cask](http://caskroom.io/) (if building for Android)
 - [xcpretty](https://github.com/supermarin/xcpretty) (`gem install xcpretty`)

## 3: Setting up a development environment & building

See the relevant SDK documentation for next steps:

* [Mapbox Android SDK](platform/android/)
* [Mapbox iOS SDK](platform/ios/)
* [Mapbox macOS SDK](platform/macos/)
* [Mapbox Qt SDK](platform/qt/)
* [Mapbox GL Native on Linux](platform/linux/)
* [node-mapbox-gl-native](platform/node/)

## 4: Keeping up to date

This repository uses Git submodules, which should be automatically checked out when you first run a `make` command for one of the above platforms. These submodules are not updated automatically and we recommended that you run `git submodule update` after pulling down new commits to this repository.
