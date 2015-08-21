# Install

This document describes the process for setting up Mapbox GL for development _on the framework itself_. 

# Depends

 - Modern C++ compiler that supports `-std=c++14` (clang++ 3.5 or later or g++-4.9 or later)
 - [Boost headers](http://boost.org/)
 - [`zlib`](http://www.zlib.net/)
 - [`libpng`](http://www.libpng.org/pub/png/libpng.html)
 - [`libuv 0.10+`](https://github.com/joyent/libuv)
 - [`glfw 3.1+`](http://www.glfw.org/)
 - [`libcurl`](http://curl.haxx.se/libcurl/) (depends on OpenSSL; Linux only)
 - Apple Command Line Tools (for build on OS X; available at [Apple Developer](https://developer.apple.com/downloads))
 - `pkg-config` (for build only)
 - [Homebrew](http://brew.sh) (for building on OS X)
 - [xcpretty](https://github.com/supermarin/xcpretty) (for building on OS X)
 - [Cask](http://caskroom.io/) (for building Android on OS X)
 - Python 2.x (for build only)

# Build instructions

We try to link to as many system-provided libraries as possible. When these are unavailable or too outdated, we run a thin build-script layer called [Mason](https://github.com/mapbox/mason) to automate builds, and load precompiled binary packages when possible.

Be sure to pull down all submodules first:

    git submodule init
    git submodule update

## OS X

This process gives you either a native OS X windowed app or a Linux desktop app with a map view for development purposes, built on an OS X host system. 

### Build

ImageMagick is required for tests. It's recommended to install it with [Homebrew](http://brew.sh/) via `brew install imagemagick`.

To create projects, you can run:

- `make xproj`: Creates an Xcode project with OS X-specific handlers for HTTP downloads and settings storage. It uses [GLFW](http://www.glfw.org) for window handling.
- `make lproj`: Creates an Xcode project with platform-independent handlers for downloads and settings storage. This is what is also being built on Linux.
- `make osx run-osx`: Builds and runs the OS X application on the command line with `xcodebuild`.
- `make linux run-linux`: Builds and runs the Linux application with `make`.

Note that you can't have more than one project in Xcode open at a time since the static library project is shared across the OS X, Linux and iOS project.

### Test

- `make test-*` Builds and runs all tests. You can specify individual tests by replacing * with their name.

### Usage

- Press `S` to cycle through bundled styles
- Press `X` to reset the transform
- Press `N` to reset north
- Press `Tab` to toggle debug information
- Press `C` to toggle symbol collision debug boxes
- Press `Esc` to quit
- Press `1` through `6` to add increasing numbers of point annotations for testing

### Target

OS X 10.9+


## iOS (Test App)

This process gives you a native iOS app for simulator or device which compiles the source to GL within it for fastest turnaround time during development. Normal, production use of the library is via a precompiled binary linked into the app target. 

Developing for iOS implies an OS X host system. 

### Build

If you want to build from source and/or contribute to development of the project, run `make iproj`, which will create and open an Xcode project which can build the entire library from source as well as an Objective-C test app. If you don't have an Apple Developer account, change the destination from "My Mac" to a simulator such as "iPhone 6" before you run and build the app.

### Test

You can run `make itest` to run the included integration tests. Requires `gem install xcpretty`. If you want to run the tests in Xcode instead, first `make ipackage` to create a local static library version, then open `test/ios/ios-tests.xcodeproj`, and lastly `Command + U` on the `Mapbox GL Tests` application target.

### Usage

- Pan to move
- Pinch to zoom
- Use two fingers to rotate
- Double-tap to zoom in one level
- Two-finger single-tap to zoom out one level
- Double-tap, long-pressing the second, then pan up and down to "quick zoom" (iPhone only, meant for one-handed use)
- Use the debug menu to add test annotations, reset position, and toggle debug info. 

## iOS (Your App)

**If you want a production version of this project, use [Mapbox Mobile](http://mapbox.com/mobile).** 

This section is for people contributing to Mapbox GL directly in the context of their own app. 

### Build

1. Install [appledoc](http://appledoc.gentlebytes.com/appledoc/) 2.2v963 for API docs generation (only this exact version works right now ([#2117](https://github.com/mapbox/mapbox-gl-native/issues/2117)).

   ```
   curl -L -o appledoc.zip https://github.com/tomaz/appledoc/releases/download/v2.2-963/appledoc.zip
   unzip appledoc.zip
   cp appledoc /usr/local/bin
   cp -Rf Templates/ ~/.appledoc
   ```
     
1. Run `make ipackage`. The packaging script will produce the statically-linked `libMapbox.a`, `Mapbox.bundle` for resources, a `Headers` folder, and a `Docs` folder with HTML API documentation.

### Test

In the context of your own app, you can now either: 

#### CocoaPods

Currently, until [#1437](https://github.com/mapbox/mapbox-gl-native/issues/1437) is completed, to install a _development version_ of Mapbox GL using CocoaPods you will need to build it from source manually per above. 

1. Zip up the build product. 

    cd build/ios/pkg/static
    ZIP=mapbox-ios-sdk.zip
    rm -f ../${ZIP}
    zip -r ../${ZIP} *

1. Modify a custom `Mapbox-iOS-SDK.podspec` to download this zip file. 

    {...}
    
    m.source = {
        :http => "http://{...}/mapbox-ios-sdk.zip",
        :flatten => true
    }
    
    {...}

1. Update your app's `Podfile` to point to the `Mapbox-iOS-SDK.podspec`. 

    pod 'Mapbox-iOS-SDK', :podspec => 'http://{...}/Mapbox-iOS-SDK.podspec'

1. Run `pod update` to grab the newly-built library. 

#### Binary

1. Built from source manually per above. 

1. Copy the contents of `build/ios/pkg/static` into your project. It should happen automatically, but ensure that:

   - `Headers` is in your *Header Search Paths* (`HEADER_SEARCH_PATHS`) build setting.
   - `Mapbox.bundle` is in your target's *Copy Bundle Resources* build phase.
   - `libMapbox.a` is in your target's *Link Binary With Libraries* build phase.

1. Add the following Cocoa framework dependencies to your target's *Link Binary With Libraries* build phase:

   - `CoreTelephony.framework`
   - `GLKit.framework`
   - `ImageIO.framework`
   - `MobileCoreServices.framework`
   - `QuartzCore.framework`
   - `SystemConfiguration.framework`
   - `libc++.dylib`
   - `libsqlite3.dylib`
   - `libz.dylib`

1. Add `-ObjC` to your target's "Other Linker Flags" build setting (`OTHER_LDFLAGS`).

### Target

Devices: iPhone 4S and above (5, 5c, 5s, 6, 6 Plus), iPad 2 and above (3, 4, Mini, Air, Mini 2, Air 2), iPod touch 5th gen and above

iOS: 7.0+


## Linux

We are using Ubuntu for development. While the software should work on other distributions as well, we are not providing explicit build instructions here.

This process gives you a Linux desktop app built on a Linux host system. 

### Build

Install GCC 4.9+ if you are running Ubuntu 14.04 or older. Alternatively, you can also use [Clang 3.5+](http://llvm.org/apt/).

    sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
    sudo apt-get update
    sudo apt-get install gcc-4.9 g++-4.9
    export CXX=g++-4.9

Ensure you have git and other build essentials:

    sudo apt-get install curl git build-essential zlib1g-dev automake \
                         libtool xutils-dev make cmake pkg-config python-pip \
                         libcurl4-openssl-dev libpng-dev libsqlite3-dev \
                         libllvm3.4

Install glfw3 dependencies:

    sudo apt-get install libxi-dev libglu1-mesa-dev x11proto-randr-dev \
                         x11proto-xext-dev libxrandr-dev \
                         x11proto-xf86vidmode-dev libxxf86vm-dev \
                         libxcursor-dev libxinerama-dev

Then, you can then proceed to build the library:

    git submodule update --init
    make linux

Set an access token as described below, and then run:

    make run-linux

### Test

- `make test-*` Builds and runs all tests. You can specify individual tests by replacing * with their name.

### Usage

- Press `S` to cycle through bundled styles
- Press `X` to reset the transform
- Press `N` to reset north
- Press `Tab` to toggle debug information
- Press `C` to toggle symbol collision debug boxes
- Press `Esc` to quit
- Press `1` through `6` to add increasing numbers of point annotations for testing

### Target

Ubuntu


## Android

### Build  (Linux Host)

Install a few build dependencies:

    apt-get install -y make git build-essential automake \
    libtool make cmake pkg-config lib32stdc++6 lib32z1

Install Oracle JDK 7 (requires license agreement) from http://www.oracle.com/technetwork/java/javase/downloads/jdk7-downloads-1880260.html

    export JAVA_HOME="/dir/to/jdk1.7.0_71"

Install the [Android NDK Revision 10e](https://developer.android.com/tools/sdk/ndk/index.html).

    export ANDROID_NDK_PATH="/dir/to/android-ndk-r10e"

Install the Android SDK. We recommend doing this by way of [Android Studio](https://developer.android.com/sdk/installing/studio.html).

    export ANDROID_HOME="/dir/to/android-sdk-linux"

Run:

    make android

You can then open `android/java` in Android Studio via "Import Non-Android Studio Project". 

### Build (OS X Host)

Install Oracle JDK 7+:

    brew cask install java

Install the [Android NDK Revision 10e](https://developer.android.com/tools/sdk/ndk/index.html) for 64-bit OS X:

    brew install android-ndk

This will also install the dependency `android-sdk`.

Install [Android Studio](https://developer.android.com/sdk/installing/studio.html):

    brew cask install android-studio
    android

By default, the SDK will be installed to `/usr/local/opt/android-sdk`. If you open Android Studio at this point, you may get an error message telling you that it can't find a JVM, it's because you installed a custom Java VM from Oracle. Follow [these instructions](http://tools.android.com/recent/androidstudio1rc3_releasecandidate3released) to start Android Studio. You'll wind up setting these environment variables in your .bash_profile or similar:

    echo "export ANDROID_HOME=`brew --prefix android-sdk`" >> .bash_profile
    echo "export ANDROID_NDK_PATH=`brew --cellar android-ndk`/r10e" >> .bash_profile
    # Replace <path to JDK> with something like /Library/Java/JavaVirtualMachines/jdk1.8.0_31.jdk
    echo "export JAVA_HOME=<path to JDK>" >> .bash_profile
    echo "export STUDIO_JDK=$JAVA_HOME" >> .bash_profile

Run:

    make android
    open -a Android\ Studio

You can then open `android/java` in Android Studio via "Import Non-Android Studio Project".

### Setting up Android emulator

If you want to run the test app in the emulator, we recommend the x86 build because it will run a lot faster.

First ensure you have an `MAPBOX_ACCESS_TOKEN` environment variable set, as described below. Then, create an x86 build:

    ANDROID_ABI=x86 make android

In Android Studio, create an x86 AVD (Android Virtual Device):

- Open AVD Manager via the Tools menu -> Android -> AVD Manager
- Click "Create Virtual Device" at the bottom on AVD Manager window
- Select one of the device profiles, for example the Nexus 4
- Click "Next"
- Select a Lollipop or Kitkat release with ABI of x86. If the line is greyed out click Download to download the OS files.
- Click "Next"
- Under "Emulated Performance" check "Host GPU" and uncheck "Store a snapshot for faster startup"
- Click "Finish"
- Close the AVD Manager

Now when you run or debug the Android project you will see a window "Choose Device". Select your new AVD from drop down under "Launch emulator". If you select "Use same device for future launches" Android Studio will remember the selection and not ask again.

### Running on a hardware device

First read the [Google documentation](http://developer.android.com/tools/device.html) to set up your device and your OS to connect to the device.

When you plug your device in and then run or debug the Android project you will see a window "Choose Device". Choose your device from the "Choose a running device" list.

If your device does not show up,  you have not set it up properly. Double check the [Google documentation](http://developer.android.com/tools/device.html).

### Target

Devices running Android 4.0+ (API level 14+).


# Troubleshooting

To trigger a complete rebuild, run `make clean` and then start over generating the Xcode projects or `Makefile` as described above.

If you are having trouble getting the dependencies right, you can blow away the `mason_packages` directory, or run `make distclean`. This means the `Makefile` and configure script will automatically install the dependencies again on the next try.

On OS X, you can also try clearing the Xcode cache with `make clear_xcode_cache`.

# Mapbox API access tokens

The demo applications use Mapbox vector tiles, which **require a Mapbox account and API access token**. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/account/apps/).

For iOS and OS X use of the demo apps in Xcode, setup the access token by editing the scheme for the application target, then adding an environment variable with the name `MAPBOX_ACCESS_TOKEN`.

![edit scheme](https://cloud.githubusercontent.com/assets/98601/5460702/c4610262-8519-11e4-873a-8597821da468.png)

![setting access token in Xcode scheme](https://cloud.githubusercontent.com/assets/162976/5349358/0a086f00-7f8c-11e4-8433-bdbaccda2b58.png)

For Linux, set the environment variable `MAPBOX_ACCESS_TOKEN` to your token.

For Android, gradle will take the value of `MAPBOX_ACCESS_TOKEN` and save it to `android/java/MapboxGLAndroidSDKTestApp/src/main/res/raw/token.txt` where the app will read it from.

# Style

Default styles in the JSON-based [styling format](https://github.com/mapbox/mapbox-gl-style-spec) are included at `./styles` as a submodule. See the [styles project](https://github.com/mapbox/mapbox-gl-styles) to report problems or to contribute to these styles.
