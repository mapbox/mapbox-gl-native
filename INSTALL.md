# Building & Developing Mapbox GL Native from Source

**Just trying to use Mapbox GL Native? You don't need to read this stuff! We
provide [easy-to-install, prebuilt versions of the Mapbox Maps SDKs for iOS and Android
that you can download instantly and get started with fast](https://www.mapbox.com/install/).**

Still with us? These are the instructions you'll need to build Mapbox GL Native
from source on a variety of platforms and set up a development environment.

Your journey will start with installing dependencies, then getting the source code, and
then setting up a development environment, which varies depending on your
operating system and what platform you want to develop for.

## 1: Installing dependencies

### macOS

 1. Install [Xcode](https://developer.apple.com/xcode/)
 2. Launch Xcode and install any updates
 3. Install [Homebrew](http://brew.sh)
 4. Install [Node.js](https://nodejs.org/), [CMake](https://cmake.org/), and [ccache](https://ccache.samba.org) with `brew install nodejs cmake ccache`
 5. Install [xcpretty](https://github.com/supermarin/xcpretty) with `[sudo] gem install xcpretty` (optional, used for prettifying command line builds)

### Linux

Install the following:

 - `clang++` 3.5 or later or `g++` 4.9 or later
 - [git](https://git-scm.com/)
 - [CMake](https://cmake.org/) 3.1 or later
 - [cURL](https://curl.haxx.se)
 - [Node.js](https://nodejs.org/) 4.2.1 or later
 - [`libcurl`](http://curl.haxx.se/libcurl/) (depends on OpenSSL)
 - [ccache](https://ccache.samba.org) (optional, improves recompilation performance)

## 2: Getting the source

 Clone the git repository:

     git clone https://github.com/mapbox/mapbox-gl-native.git
     cd mapbox-gl-native

## 3: Setting up a development environment & building

See the relevant SDK documentation for next steps:

* [Maps SDK for Android](platform/android/)
* [Maps SDK for iOS](platform/ios/)
* [Maps SDK for macOS](platform/macos/)
* [Mapbox Qt SDK](platform/qt/)
* [Mapbox GL Native on Linux](platform/linux/)
* [node-mapbox-gl-native](platform/node/)

## 4: Keeping up to date

This repository uses Git submodules, which should be automatically checked out when you first run a `make` command for one of the above platforms. These submodules are not updated automatically and we recommended that you run `git submodule update` after pulling down new commits to this repository.
