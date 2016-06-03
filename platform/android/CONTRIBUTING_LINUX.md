# Contributing to the Android SDK on Linux

_These instructions were tested on Ubuntu 16.04 LTS (aka Xenial Xerus)._

Install the build dependencies:

```
$ sudo apt-get install -y android-tools-adb build-essential curl git \
        lib32stdc++6 lib32z1 openjdk-8-jdk pkg-config python
```

Install the Android SDK. We recommend doing this by way of [Android command line tools for Linux](http://developer.android.com/sdk/index.html) but you can also achieve same results with the Android Studio package.

Unpack the SDK and make sure you have the proper environment variables set.

```
$ export PATH=/path/to/android-sdk-linux/tools:$PATH
$ export ANDROID_HOME=/path/to/android-sdk-linux
```

Update the Android SDK to the latest version:

```
$ android update sdk -u
```

## Setting Mapbox Access Token

_The demo applications use Mapbox vector tiles, which require a Mapbox account and API access token. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/)._

gradle will take the value of the `MAPBOX_ACCESS_TOKEN` environ variable and save it to `platform/android/MapboxGLAndroidSDKTestApp/src/main/res/values/developer-config.xml` where the app will read it from.  Otherwise, you can edit `developer-config.xml` and add the value manually as `mapbox_access_token`.

## Building

Checking out the code:

```
$ git clone https://github.com/mapbox/mapbox-gl-native.git
$ cd mapbox-gl-native
```

Building a debug version will generated a self-signed test application that can be installed on the phone or emulator:

```
$ BUILDTYPE=Debug make android
$ adb install -r platform/android/MapboxGLAndroidSDKTestApp/build/outputs/apk/MapboxGLAndroidSDKTestApp-debug.apk 
```

The debug version will emit considerable more log information (and run slower). Use simply `make android` to build a release version.
