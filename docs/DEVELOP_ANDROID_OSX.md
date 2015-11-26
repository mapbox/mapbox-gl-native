# Developing for Android on OS X

Install Oracle JDK 7+ and Android Studio:

    brew tap caskroom/cask
    brew install brew-cask
    brew cask install java

    brew cask install android-studio


Once Android Studio is downloaded and installed, the [first time it's run it'll ask to install the Android SDK](http://developer.android.com/sdk/installing/index.html?pkg=studio) which you should do.  While doing so in the Android SDK Manager make sure to also select and install the latest versions of "Android Support Repository" and "Android Support Library" from "Extras":

![image](https://cloud.githubusercontent.com/assets/98601/9915837/289f398e-5c6e-11e5-9a84-ed4d08d52d1f.png)

By default, the Android SDK will be installed to `/Users/<user>/Library/Android/sdk/`.  For more information on how to [configure Android Studio](http://tools.android.com/tech-docs/configuration) and how to [set Project JDK vs IDE JDK](http://tools.android.com/tech-docs/configuration/osx-jdk) please see [Google's documentation](http://tools.android.com/overview).

## Setting Mapbox Access Token

_The test application (used for development purposes) uses Mapbox vector tiles, which require a Mapbox account and API access token. Obtain a free access token on the [Mapbox account page](https://www.mapbox.com/account/apps/)._

gradle will take the value of the `MAPBOX_ACCESS_TOKEN` environ variable and save it to `MapboxGLAndroidSDKTestApp/src/main/res/values/developer-config.xml` where the app will read it from.

## Developing In Android Studio

To work with the Mapbox Android SDK, you'll first need to get it setup as a Project in Android Studio.  To do so Open Android Studio and select "Import project (Eclipse ADT, Gradle, etc.)" from the Welcome to Android Studio dialog.  From there select the `android` directory from the local file system where `mapbox-gl-native` was cloned.  For example:

```sh
/Users/<user>/development/mapbox-gl-native/android
```

The Mapbox Android SDK is a multi-module Gradle based project.  Specifically, the SDK itself is an [Android Library](http://developer.android.com/tools/projects/index.html#LibraryModules) module and it utilizes a companion [test module](http://developer.android.com/tools/projects/index.html#testing) (aka "the TestApp") for daily development.  When Android Studio finishes importing the project both `MapboxGLAndroidSDK` and `MapboxGLAndroidSDKTestApp` modules should be visible.


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

**Next: get your app [running on a hardware Android Device](docs/ANDROID_DEVICE.md) or [simulator](docs/ANDROID_SIMULATOR.md)**
