[![Travis](https://api.travis-ci.org/mapbox/mapbox-gl-native.svg?branch=master)](https://travis-ci.org/mapbox/mapbox-gl-native/builds)

An OpenGL renderer for [Mapbox Vector Tiles](https://www.mapbox.com/blog/vector-tiles),
implemented in C++11, currently targeting iOS, OS X, and Ubuntu Linux.

# Depends

 - Modern C++ compiler that supports `-std=c++11` (On OS X clang++, on Linux g++-4.8 or g++-4.9)
 - [Boost headers](http://boost.org/)
 - [`zlib`](http://www.zlib.net/)
 - [`libpng`](http://www.libpng.org/pub/png/libpng.html)
 - [`libuv 0.10+`](https://github.com/joyent/libuv)
 - [`glfw 3.1+`](http://www.glfw.org/)
 - [`libcurl`](http://curl.haxx.se/libcurl/) (depends on OpenSSL; Linux only)
 - Apple Command Line Tools (for build on OS X; available at [Apple Developer](https://developer.apple.com/downloads))
 - `pkg-config` (for build only)
 - [Homebrew](http://brew.sh) (for build on OS X)
 - Python 2.x (for build only)

# Build instructions

We try to link to as many system-provided libraries as possible. When these are unavailable or too outdated, we run a thin build-script layer called [Mason](https://github.com/mapbox/mason) to automate builds, and load precompiled binary packages when possible.

## OS X

On OS X, we are using `zlib`, `SQLite3`, `libcurl` and `libpng` provided by the operating system. In addition to that, you need to have the Boost headers installed. To install all prerequisites, use [Homebrew](http://brew.sh/) and type `brew install pkg-config boost`.

To create projects, you can run:

- `make xproj`: Creates an Xcode project with OS X-specific handlers for HTTP downloads and settings storage. It uses [GLFW](http://www.glfw.org) for window handling.
- `make lproj`: Creates an Xcode project with platform-independent handlers for downloads and settings storage. This is what is also being built on Linux.
- `make osx run-osx`: Builds and runs the OS X application on the command line with `xcodebuild`.
- `make linux run-linux`: Builds and runs the Linux application with `make`.

Note that you can't have more than one project in Xcode open at a time since they the static library project is shared across the OS X, Linux and iOS project.

Target OS: 10.9+


## iOS

iOS makes use of a Cocoa-specific API called [`mapbox-gl-cocoa`](https://github.com/mapbox/mapbox-gl-cocoa). If you are just interested in running Mapbox GL on iOS and not developing with it, head to that project and you can use this library as a pre-built static library instead. A `UIView` interface to the map view and bundle resources are provided there.

If you intend to develop here, `mapbox-gl-cocoa` is included as a submodule of the overall build setup.

To pull down the submodule(s), run:

    git submodule init
    git submodule update

Then, you can run `make iproj` to create and open an Xcode project with an iOS-specific view controller housing. This will automatically install required dependencies as well.

Note that if you are doing OS X development as well, to toggle from OS X back to iOS, you will need to `make iproj` again.

Target devices: iPhone 4S and above (5, 5c, 5s, 6, 6 Plus) and iPad 2 and above (3, 4, Mini , Air, Mini 2, Air 2).

Target OS: 7.0+

## Linux

We are using Ubuntu for development. While the software should work on other distributions as well, we are not providing explicit build instructions here.

Install GCC 4.8+ if you are running Ubuntu 13.10 or older. Alternatively, you can also use Clang 3.4+.

	sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
	sudo apt-get update
	sudo apt-get install gcc-4.8 g++-4.8

Ensure you have git and other build essentials:

    sudo apt-get install git build-essential zlib1g-dev automake \
                         libtool xutils-dev make cmake pkg-config python-pip \
                         libcurl4-openssl-dev libpng-dev libsqlite3-dev

Install glfw3 dependencies:

    sudo apt-get install libxi-dev libglu1-mesa-dev x11proto-randr-dev \
                         x11proto-xext-dev libxrandr-dev \
                         x11proto-xf86vidmode-dev libxxf86vm-dev \
                         libxcursor-dev libxinerama-dev

Finally, install Boost. If you're running Ubuntu 12.04 or older, you need to install a backport PPA since the version provided by APT doesn't contain Boost Geometry:

	sudo add-apt-repository --yes ppa:boost-latest/ppa
	sudo apt-get update
	sudo apt-get install libboost1.55-dev

Otherwise, you can just install

	sudo apt-get install libboost-dev

Once you're done installing the build dependencies, you can get started by running

	./configure

which downloads all other dependencies that we need to build manually with [Mason](https://github.com/mapbox/mason). There's a good chance that there already are binary files and that you don't need to compile anything.

Then, you can then proceed to build the library like:

    make mbgl

Or proceed to building the debug application with:

	make linux

## Android (on Linux)

Follow Linux steps until ./configure

Install Oracle JDK 7

	wget http://download.oracle.com/otn-pub/java/jdk/7u71-b14/jdk-7u71-linux-x64.tar.gz
	tar -xzf jdk-7u71-linux-x64.tar.gz
	export JAVA_HOME="/dir/to/jdk1.7.0_71"

Install Android NDK

	wget http://dl.google.com/android/ndk/android-ndk-r10c-linux-x86_64.bin
	chmod +x android-ndk-r10c-linux-x86_64.bin
	./android-ndk-r10c-linux-x86_64.bin
	export ANDROID_NDK_PATH="/dir/to/android-ndk-r10c"
	export NDK_PATH=$(ANDROID_NDK_PATH)

Install Android SDK

	wget http://dl.google.com/android/android-sdk_r23.0.2-linux.tgz
	tar -xzf ./android-sdk_r23.0.2-linux.tgz
    ./android-sdk-linux/tools/android update sdk -u -a -t tools,platform-tools,build-tools-21.1.0,android-19,android-21,extra-android-m2repository,extra-google-m2repository
    export ANDROID_HOME="/dir/to/android-sdk-linux"

Run

	make android

APKs for testing are output to android/java/app/build/outputs/apk/app-debug.apk

You can also open android/java in Android Studio.

# Troubleshooting

To trigger a complete rebuild, run `make clean` and then start over generating the Xcode projects or Makefiles as described above.

If you are having trouble getting the dependencies right, you can blow away the `mason_packages` directory, or run `make distclean`. This means the Makefile and configure script will automatically install the dependencies again on the next try.

On OS X, you can also try clearing the Xcode cache with `make clear_xcode_cache`.

# Mapbox API access tokens

The demo applications use Mapbox vector tiles, which **require a Mapbox account and API access token**. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/account/apps/).

For iOS and OS X use of the demo apps in Xcode, setup the access token by editing the scheme for the application target, then adding an environment variable with the name `MAPBOX_ACCESS_TOKEN`.

![edit scheme](https://cloud.githubusercontent.com/assets/98601/3647749/30f74f26-1102-11e4-84af-f1be853b4e38.png)

![setting access token in Xcode scheme](https://cloud.githubusercontent.com/assets/52399/3543326/9e7cfbb8-0860-11e4-8def-3684a9028b61.png)

For Linux, set the environment variable `MAPBOX_ACCESS_TOKEN` to your token.

# Style

Some styles in JSON format are included at `./styles`. See the [style spec](https://github.com/mapbox/mapbox-gl-style-spec) for more details.

# Usage

## Desktop

- Press `X` to reset the transform
- Press `N` to reset north
- Press `Tab` to toggle debug information
- Press `Esc` to quit

## Mobile

- Pan to move
- Pinch to zoom
- Use two fingers to rotate
- Double-tap to zoom in one level
- Two-finger single-tap to zoom out one level
- Single-tap to toggle the command palette visibility for resetting north & the transform, toggling debug, and locating the user
- Double-tap, long-pressing the second, then pan up and down to "quick zoom" (iPhone only, meant for one-handed use)

# Other notes

Under early development, this project was called LLMR (Low-Level Map Renderer), in case you see any lingering references to it.
