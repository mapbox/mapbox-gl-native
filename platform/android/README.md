# [Mapbox Android SDK](https://www.mapbox.com/android-sdk/)

[![Bitrise](https://www.bitrise.io/app/79cdcbdc42de4303.svg?token=_InPF8bII6W7J6kFr-L8QQ&branch=master)](https://www.bitrise.io/app/79cdcbdc42de4303)

A library based on [Mapbox GL Native](../../README.md) for embedding interactive map views with scalable, customizable vector maps into Java applications on Android devices.

**To install and use the Mapbox Android SDK in an application, see the [Mapbox Android SDK website](https://www.mapbox.com/android-sdk/).**

[![](https://www.mapbox.com/android-sdk/images/splash.png)](https://www.mapbox.com/android-sdk/)

## Contributing to the SDK

**These instructions are for developers interested in making code-level contributions to the SDK itself. If you instead want to use the SDK in your app, see above.**

Building the SDK yourself requires [a number of dependencies and steps](../../INSTALL.md) that are unnecessary for developing production applications.

* [Contributing on Linux](CONTRIBUTING_LINUX.md)
* [Contributing on macOS](CONTRIBUTING_MACOS.md)

### Setting up the Android emulator

The Mapbox Android SDK requires Android 4.0.3+ (API level 15+).

If you want to run the test app in the emulator, we recommend the x86 build because it will run a lot faster.

First ensure you have an `MAPBOX_ACCESS_TOKEN` environment variable set, as described below. Then, create an x86 build:

    make android-lib-x86

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

### Running Mapbox GL Native on a hardware Android device

The Mapbox Android SDK requires Android 4.0.3+ (API level 15+).

First read the [Google documentation](http://developer.android.com/tools/device.html) to set up your device and your OS to connect to the device.

When you plug your device in and then run or debug the Android project you will see a window "Choose Device". Choose your device from the "Choose a running device" list.

If your device does not show up, double check the [Google documentation](http://developer.android.com/tools/device.html).
