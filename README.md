[![Travis](https://api.travis-ci.org/mapbox/mapbox-gl-native.svg)](https://travis-ci.org/mapbox/mapbox-gl-native/builds)

An OpenGL renderer for [Mapbox vector tiles](https://www.mapbox.com/blog/vector-tiles),
implemented in C++11, currently targeting iOS, OS X, and Ubuntu Linux.

# Depends

 - Modern C++ compiler that supports `-std=c++11` (On OSX clang++, on Linux g++-4.8 or g++-4.9)
 - Boost headers
 - `libpng`
 - `libuv`
 - `glfw3`
 - `libcurl` (depends on OpenSSL; Linux only)
 - `libboost_regex` (Linux only)
 - Homebrew (for build on OS X)
 - Python 2.x (for build only)
 - Node.js (for build only)

# Build instructions

We use [mapnik-packaging](https://github.com/mapnik/mapnik-packaging) to build static libraries of
dependencies.

First off: if you hit problems during development try:

    make clean

This will clear cached build data and update to the latest versions of dependencies.

## OS X

First run:

    make setup

This downloads all required dependencies, builds them and creates universal libraries that can be used on both OS X and iOS.

To create projects, you can run:
- `make xproj`: Creates an Xcode project with OS X-specific handlers for HTTP downloads and
  settings storage. It uses [GLFW](http://www.glfw.org) for window handling.
- `make lproj`: Creates an Xcode project with platform-independent handlers for downloads
  and settings storage. This is what is also being built on Linux.
- `make linux`: Builds the Linux GLFW application with `make`.

## iOS

iOS makes use of a Cocoa-specific API called [mapbox-gl-cocoa](https://github.com/mapbox/mapbox-gl-cocoa), 
which is included as a submodule and provides a `UIView` interface to the map view and some bundle resources. 

First, pull down the submodule(s): 

    git submodule init
    git submodule update

Then run:

    make setup

This downloads all required dependencies, builds them and creates universal libraries that can be used on both OS X and iOS.

Lastly, `make iproj` to create and open an Xcode project with an iOS-specific view controller housing. 

Target devices: iPhone 4 and above (4S, 5, 5c, 5s) and iPad 2 and above (3, 4, mini and/or retina).

## Ubuntu

Ensure you have git and other build essentials:

    sudo apt-get update
    sudo apt-get install git build-essential zlib1g-dev automake libtool xutils-dev make cmake pkg-config nodejs-legacy curl

Install a `-std=c++11` capable compiler

    sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
    sudo apt-get update
    sudo apt-get install gcc-4.8 g++-4.8

Install glfw3 dependencies:

    sudo apt-get install libxi-dev libglu1-mesa-dev x11proto-randr-dev x11proto-xext-dev libxrandr-dev x11proto-xf86vidmode-dev libxxf86vm-dev libxcursor-dev

Then run:

    make setup

This downloads all required dependencies, builds them and creates universal libraries that can be used on both OS X and iOS.

You can then proceed to build the library like:

    make linux

# Style

The default stylesheet at `bin/style.js` is JSON and is processed into a minified version by the following script: 

```
node bin/build-style.js bin/style.js out/
```

Note: This is automatically taken care of as a build phase so you are not expected to do this manually.

# Usage

## Desktop

- Press `X` to reset the transform
- Press `N` to reset north
- Press `R` to toggle styles
- Press `Tab` to toggle debug information
- Press `Esc` to quit

## Mobile

- Pan to move
- Pinch to zoom
- Use two fingers to rotate
- Double-tap to zoom in one level
- Two-finger single-tap to zoom out one level
- Single-tap to toggle the command palette visibility for resetting north & the transform, toggling debug, toggling styles, and locating the user
- Double-tap, long-pressing the second, then pan up and down to "quick zoom" (iPhone only, meant for one-handed use)

# Other notes

Under early development, this project was called LLMR (Low-Level Map Renderer), in case you see any lingering references to it. 
