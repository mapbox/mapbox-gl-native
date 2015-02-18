[![Travis](https://api.travis-ci.org/mapbox/mapbox-gl-native.svg?branch=master)](https://travis-ci.org/mapbox/mapbox-gl-native/builds)

An OpenGL renderer for [Mapbox Vector Tiles](https://www.mapbox.com/blog/vector-tiles),
implemented in C++11, currently targeting iOS, OS X, Android, and Ubuntu Linux.

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
 - [Homebrew](http://brew.sh) (for building on OS X)
 - [Cask](http://caskroom.io/) (for building Android on OS X)
 - Python 2.x (for build only)

# Build instructions

We try to link to as many system-provided libraries as possible. When these are unavailable or too outdated, we run a thin build-script layer called [Mason](https://github.com/mapbox/mason) to automate builds, and load precompiled binary packages when possible.

Be sure to pull down all submodules first:

    git submodule init
    git submodule update

## OS X

Prerequisites include the Boost headers (for routines provided therein) and ImageMagick (for tests). To install all prerequisites, use [Homebrew](http://brew.sh/) and type `brew install pkg-config boost imagemagick`.

To create projects, you can run:

- `make xproj`: Creates an Xcode project with OS X-specific handlers for HTTP downloads and settings storage. It uses [GLFW](http://www.glfw.org) for window handling.
- `make lproj`: Creates an Xcode project with platform-independent handlers for downloads and settings storage. This is what is also being built on Linux.
- `make osx run-osx`: Builds and runs the OS X application on the command line with `xcodebuild`.
- `make linux run-linux`: Builds and runs the Linux application with `make`.
- `make test-*` Builds and runs all tests. You can specify individual tests by replacing * with their name.

Note that you can't have more than one project in Xcode open at a time since they the static library project is shared across the OS X, Linux and iOS project.

Target OS X: 10.9+


## iOS

If you merely want to install the library for iOS and try it out as an Objective-C consumer, run `./scripts/package_ios.sh`. It requires the Boost headers to be installed, so use [Homebrew](http://brew.sh/) to install them (`brew install boost`). The packaging script will produce the statically-linked `libMapboxGL.a`, `MapboxGL.bundle` for resources, and a `Headers` folder.

If you want to build from source and/or contribute to development of the project, run `make iproj`, which will create and open an Xcode project which can build the entire library from source as well as an Objective-C test app.

Target devices: iPhone 4S and above (5, 5c, 5s, 6, 6 Plus) and iPad 2 and above (3, 4, Mini, Air, Mini 2, Air 2).

Target iOS: 7.0 through 8.1


## Linux

We are using Ubuntu for development. While the software should work on other distributions as well, we are not providing explicit build instructions here.

Install GCC 4.8+ if you are running Ubuntu 13.10 or older. Alternatively, you can also use Clang 3.4+.

    sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
    sudo apt-get update
    sudo apt-get install gcc-4.8 g++-4.8

Ensure you have git and other build essentials:

    sudo apt-get install curl git build-essential zlib1g-dev automake \
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

Then, you can then proceed to build the library:

	git submodule update --init
	make linux

Set an access token as described below, and then run:

	make run-linux

## Android

Target devices: All Android devices on Android 4.0 or newer (API level >= 14).

### On Linux

Install a few build dependencies:

    apt-get install -y make git build-essential automake \
    libtool make cmake pkg-config lib32stdc++6 lib32z1

Install Oracle JDK 7 (requires license agreement) from http://www.oracle.com/technetwork/java/javase/downloads/jdk7-downloads-1880260.html

    export JAVA_HOME="/dir/to/jdk1.7.0_71"

Install the [Android NDK Revision 10d](https://developer.android.com/tools/sdk/ndk/index.html).

    export ANDROID_NDK_PATH="/dir/to/android-ndk-r10d"

Install the Android SDK. We recommend doing this by way of [Android Studio](https://developer.android.com/sdk/installing/studio.html).

    export ANDROID_HOME="/dir/to/android-sdk-linux"

Run:

    make android

You can then open `android/java` in Android Studio via "Import Non-Android Studio Project".

### On OS X

Install Oracle JDK 7+:

    brew cask install java

Install the [Android NDK Revision 10d](https://developer.android.com/tools/sdk/ndk/index.html) for 64-bit OS X:

    brew install android-ndk

This will also install the dependency `android-sdk`.

Install [Android Studio](https://developer.android.com/sdk/installing/studio.html):

    brew cask install android-studio
    android

By default, the SDK will be installed to `/usr/local/opt/android-sdk`. If you open Android Studio at this point, you may get an error message telling you that it can't find a JVM, it's because you installed a custom Java VM from Oracle. Follow [these instructions](http://tools.android.com/recent/androidstudio1rc3_releasecandidate3released) to start Android Studio. You'll wind up setting these environment variables in your .bash_profile or similar:

    echo "export ANDROID_HOME=`brew --prefix android-sdk`" >> .bash_profile
    echo "export ANDROID_NDK_PATH=`brew --cellar android-ndk`/r10d" >> .bash_profile
    # Replace <path to JDK> with something like /Library/Java/JavaVirtualMachines/jdk1.8.0_31.jdk
    echo "export JAVA_HOME=<path to JDK>" >> .bash_profile
    echo "export STUDIO_JDK=$JAVA_HOME" >> .bash_profile

Run:

    make android
    open -a Android\ Studio

You can then open `android/java` in Android Studio via "Import Non-Android Studio Project".

# Troubleshooting

To trigger a complete rebuild, run `make clean` and then start over generating the Xcode projects or Makefiles as described above.

If you are having trouble getting the dependencies right, you can blow away the `mason_packages` directory, or run `make distclean`. This means the Makefile and configure script will automatically install the dependencies again on the next try.

On OS X, you can also try clearing the Xcode cache with `make clear_xcode_cache`.

# Mapbox API access tokens

The demo applications use Mapbox vector tiles, which **require a Mapbox account and API access token**. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/account/apps/).

For iOS and OS X use of the demo apps in Xcode, setup the access token by editing the scheme for the application target, then adding an environment variable with the name `MAPBOX_ACCESS_TOKEN`.

![edit scheme](https://cloud.githubusercontent.com/assets/98601/5460702/c4610262-8519-11e4-873a-8597821da468.png)

![setting access token in Xcode scheme](https://cloud.githubusercontent.com/assets/162976/5349358/0a086f00-7f8c-11e4-8433-bdbaccda2b58.png)

For Linux, set the environment variable `MAPBOX_ACCESS_TOKEN` to your token.

For Android, gradle will take the value of `MAPBOX_ACCESS_TOKEN` and save it to `android/java/app/src/main/res/raw/token.txt` where the app will read it from.

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
- Double-tap, long-pressing the second, then pan up and down to "quick zoom" (iPhone only, meant for one-handed use)

# Other notes

Under early development, this project was called LLMR (Low-Level Map Renderer), in case you see any lingering references to it.
