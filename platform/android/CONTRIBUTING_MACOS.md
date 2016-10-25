# Contributing to the Android SDK on macOS

## Install JDK 7+ and Android Studio:

 Android Studio requires the Java Development Kit (JDK) which you can [download here](http://www.oracle.com/technetwork/java/javase/downloads/index.html). Once the JDK is installed, you can grab the [latest version of Android Studio here](https://developer.android.com/sdk/index.html) and install it following the on screen instructions.  

Once Android Studio is installed, the [first time it's run it'll ask to install the Android SDK](http://developer.android.com/sdk/installing/index.html?pkg=studio) which you should do.  While doing so in the Android SDK Manager make sure to also select and install the latest versions of "Android Support Repository" and "Android Support Library" from "Extras":

![image](https://cloud.githubusercontent.com/assets/98601/9915837/289f398e-5c6e-11e5-9a84-ed4d08d52d1f.png)

By default, the Android SDK will be installed to `/Users/<user>/Library/Android/sdk/`.  For more information on how to [configure Android Studio](http://tools.android.com/tech-docs/configuration) and how to [set Project JDK vs IDE JDK](http://tools.android.com/tech-docs/configuration/osx-jdk) please see [Google's documentation](http://tools.android.com/overview).

## Setting Mapbox Access Token

_The test application (used for development purposes) uses Mapbox vector tiles, which require a Mapbox account and API access token. Obtain a free access token on the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/)._

If you start Android Studio from your terminal, gradle will take the value of the `MAPBOX_ACCESS_TOKEN` environment variable and save it to `MapboxGLAndroidSDKTestApp/src/main/res/values/developer-config.xml` where the app will read it from. Otherwise,
you can edit `developer-config.xml` and add the value manually as `mapbox_access_token`.

## Developing In Android Studio

To work with the Mapbox Android SDK, you'll first need to get it set up as a Project in Android Studio.  To do so Open Android Studio and select "Import project (Eclipse ADT, Gradle, etc.)" from the Welcome to Android Studio dialog.  From there select the `platform/android` directory from the local file system where `mapbox-gl-native` was cloned.  For example:

```sh
/Users/<user>/development/mapbox-gl-native/platform/android
```

The Mapbox Android SDK is a multi-module Gradle based project.  Specifically, the SDK itself is an [Android Library](http://developer.android.com/tools/projects/index.html#LibraryModules) module and it utilizes a companion [test module](http://developer.android.com/tools/projects/index.html#testing) (aka "the TestApp") for daily development.  When Android Studio finishes importing the project both `MapboxGLAndroidSDK` and `MapboxGLAndroidSDKTestApp` modules should be visible.

## Setting `ANDROID_HOME`

For `build android` to work, you'll need to set the `ANDROID_HOME` environment
to point to your Android SDK installation:

```
export ANDROID_HOME=/<installation location>/android-sdk-macosx
```

This environment variable configuration should go into a file that's read on
your shell's startup, like `~/.profile`.

## Speeding up gradle builds

To optimise you development machine for faster gradle builds create the following `/Users/name/.gradle/gradle.properties`  file:

```
org.gradle.daemon=true
org.gradle.jvmargs=-Xmx3072M
```

## Running The TestApp

In order to run the TestApp on an emulator or device the Core GL portion needs to built first.  Core GL is the common C++ based OpenGL engine that powers the maps for iOS, Android, and Qt in the project.  To build it, open Terminal and run the following commands from the root of the `mapbox-gl-native` source code

Run:

    // From /Users/<user>/development/mapbox-gl-native

    // Makes arm7 ABI version of Core GL
    // Can be run on most Android phones and arm emulator
    make android

    // Make x86 version of Core GL
    // Useful for running faster Anroid x86 emulator on Macs
    make android-lib-x86

Once Core GL has been built, the TestApp can be run by selecting `MapboxGLAndroidSDKTestApp` from the Run menu or toolbar in Android Studio.

**Next: get your app [running on a hardware Android Device](platform/android/README.md#running-mapbox-gl-native-on-a-hardware-android-device) or [simulator](platform/android/README.md#setting-up-the-android-emulator)**
