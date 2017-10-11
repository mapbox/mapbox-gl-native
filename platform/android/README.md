# [Mapbox Android SDK](https://www.mapbox.com/android-sdk/)

[![Circle CI build status](https://circleci.com/gh/mapbox/mapbox-gl-native.svg?style=shield)](https://circleci.com/gh/mapbox/workflows/mapbox-gl-native/tree/master)

A library based on [Mapbox GL Native](../../README.md) for embedding interactive map views with scalable, customizable vector maps into Java applications on Android devices.

## Getting Started 

Alright. So, actually, you may be in the wrong place. From here on in, this README is going to be for people who are interested in working on and improving on Mapbox GL Native for Android.

**To view our current API documentation, see our [JavaDoc](https://www.mapbox.com/android-sdk/api).**

**To install and use the Mapbox Android SDK in an application, see the [Mapbox Android SDK website](https://www.mapbox.com/android-sdk/).**

[![](https://www.mapbox.com/android-sdk/images/splash.png)](https://www.mapbox.com/android-sdk/)

#### Open project in Android Studio

#### Using the SDK snapshot

Instead of using the latest stable release of the Mapbox Android SDK, you can use the nightly build/"snapshot" or the beta version if there is one available. If you'd like to use a snapshot build,  your Android project's gradle file should
have -SNAPSHOT appended to the SDK version number. For example `5.2.0-SNAPSHOT` or:

```java
// Mapbox SDK dependency
    compile('com.mapbox.mapboxsdk:mapbox-android-sdk:5.2.0-SNAPSHOT@aar') {
    transitive = true
    }
```
You need to have the section below in your build.gradle root folder to be able to resolve the SNAPSHOT dependencies:
```
allprojects {
    repositories {
        jcenter()
        maven { url "http://oss.sonatype.org/content/repositories/snapshots/" }
    }
}
```


### Setup environment

**These instructions are for developers interested in making code-level contributions to the SDK itself. If you instead want to use the SDK in your app, see above.**

#### Getting the source

Clone the git repository

```bash
git clone https://github.com/mapbox/mapbox-gl-native.git
cd mapbox-gl-native
```

#### Installing dependencies

These dependencies are required for all operating systems and all platform targets.

- Latest stable [Android Studio](https://developer.android.com/studio/index.html) 
- Update Android SDK with latest
  - Android SDK Build-Tools 
  - Android Platform-Tools
  - Android SDK Tools
  - CMake
  - NDK
  - LLDB

- Modern C++ compiler that supports `-std=c++14`\*
  - clang++ 3.5 or later or
  - g++-4.9 or later
- [cURL](https://curl.haxx.se) (for build only)
- [Node.js](https://nodejs.org/) or later (for build only)
- [pkg-config](https://wiki.freedesktop.org/www/Software/pkg-config/) (for build only)

**Note**: We partially support C++14 because GCC 4.9 does not fully implement the
final draft of the C++14 standard. More information in [DEVELOPING.md](DEVELOPING.md).

##### Additional Dependencies for Linux

_These instructions were tested on Ubuntu 16.04 LTS (aka Xenial Xerus)._

```
$ sudo apt-get install -y build-essential curl lib32stdc++6 lib32z1 pkg-config python
```

##### Additional Dependencies for macOS

- Apple Command Line Tools (available at  [Apple Developer](https://developer.apple.com/download/more/))
- [xcpretty](https://github.com/supermarin/xcpretty) (`gem install xcpretty`)


#### Open project in Android Studio

We recommend that you open this repository's `android` folder in Android Studio. 

mapbox-gl-native > platform > android

Once you open it in Android Studio and switch to the Android view in the left-hand sidebar,  you should see the `MapboxGLAndroidSDK` and `MapboxGLAndroidSDKTestApp` folders.


Alternatively, you can use the Terminal application to open the project in Android Studio. Open your Terminal application, navigate to this repository's root folder, and run the `make aproj` command.


##### macOS

Execute the following to generate the required build files and open the project with Android Studio:

```
make aproj
```

##### linux

Open Android Studio project in `/platform/android`, run `make android-configuration` in the root folder of the project.

##### Setup Checkstyle

Mapbox uses specific IDE settings related to code and check style. 
See [checkstyle guide](https://github.com/mapbox/mapbox-gl-native/wiki/Setting-up-Mapbox-checkstyle) for configuration details. 

##### Setting Mapbox Access Token

_The test application (used for development purposes) uses Mapbox vector tiles, which require a Mapbox account and API access token. Obtain a free access token on the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/)._

With the first gradle invocation, gradle will take the value of the `MAPBOX_ACCESS_TOKEN` environment variable and save it to `MapboxGLAndroidSDKTestApp/src/main/res/values/developer-config.xml`. If the environement variable wasn't set, you can edit `developer-config.xml` manually and add your access token to the `mapbox_access_token` resource.  

#### Running project

Run the configuration for the `MapboxGLAndroidSDKTestApp` module and select a device or emulator to deploy on. Based on the selected device, the c++ code will be compiled for the related processor architecture. You can see the project compiling in the `View > Tool Windows > Gradle Console`. 

More information about building and distributing this project in [DISTRIBUTE.md][https://github.com/mapbox/mapbox-gl-native/blob/master/platform/android/DISTRIBUTE.md].

#### Symbolicating native crashes

When hitting native crashes you can use ndk-stack to symbolicate crashes. 
More information in [this](https://github.com/mapbox/mapbox-gl-native/wiki/Getting-line-numbers-from-an-Android-crash-with-ndk-stack) guide.