# [Mapbox Maps SDK for Android](https://www.mapbox.com/android-sdk/)

[![Circle CI build status](https://circleci.com/gh/mapbox/mapbox-gl-native.svg?style=shield)](https://circleci.com/gh/mapbox/workflows/mapbox-gl-native/tree/master)

A library based on [Mapbox GL Native](../../README.md) for embedding interactive map views with scalable, customizable vector maps onto Android devices.

## Getting Started

Alright. So, actually, you may be in the wrong place. From here on in, this README is going to be for people who are interested in working on and improving on Mapbox GL Native for Android.

**To view our current API documentation, see our [JavaDoc](https://www.mapbox.com/android-sdk/api).**

**To install and use the Mapbox Maps SDK for Android in an application, see the [Mapbox Maps SDK for Android website](https://www.mapbox.com/install/android/).**

[![](https://www.mapbox.com/android-sdk/images/splash.png)](https://www.mapbox.com/android-sdk/)

### Setup environment

**These instructions are for developers interested in making code-level contributions to the SDK itself. If you instead want to use the SDK in your app, see above.**

#### Getting the source

Clone the git repository

```bash
git clone git@github.com:mapbox/mapbox-gl-native.git && cd mapbox-gl-native
```

#### Installing dependencies

These dependencies are required for all operating systems and all platform targets.

- Latest stable [Android Studio](https://developer.android.com/studio/index.html)
- Update the Mapbox Maps SDK for Android with the latest
  - Android SDK Build-Tools
  - Android Platform-Tools
  - Android SDK Tools
  - CMake
  - NDK
  - LLDB
- Modern C++ compiler that supports `-std=c++14`\*
  - clang++ 3.5 or later or
  - g++-4.9 or later
- [Node.js](https://nodejs.org/)
  - make sure [npm](https://www.npmjs.com) is installed as well
- [ccache](https://ccache.samba.org/) (optional)

**Note**: We partially support C++14 because GCC 4.9 does not fully implement the
final draft of the C++14 standard. More information in [DEVELOPING.md](DEVELOPING.md).

**Note**: On macOS you can install clang with installing the [Apple command line developer tools](https://developer.apple.com/download/).

### Opening the project

#### macOS

Execute the following command in this repository's root folder to generate the required build files and open the project with Android Studio:

```
make aproj
```

#### linux

run `make android-configuration` in the root folder of the project and open the Android Studio project in `/platform/android`.

If you are using Arch Linux, install [ncurses5-compat-libs](https://aur.archlinux.org/packages/ncurses5-compat-libs).

### Project configuration

#### Setup Checkstyle

Mapbox uses specific IDE settings related to code and check style.
See [checkstyle guide](https://github.com/mapbox/mapbox-gl-native/wiki/Setting-up-Mapbox-checkstyle) for configuration details.

##### Setting Mapbox Access Token

_The test application (used for development purposes) uses Mapbox vector tiles, which require a Mapbox account and API access token. Obtain a free access token on the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/)._

With the first gradle invocation, gradle will take the value of the `MAPBOX_ACCESS_TOKEN` environment variable and save it to `MapboxGLAndroidSDKTestApp/src/main/res/values/developer-config.xml`. If the environment variable wasn't set, you can edit `developer-config.xml` manually and add your access token to the `mapbox_access_token` resource.  

### Running project

Run the configuration for the `MapboxGLAndroidSDKTestApp` module and select a device or emulator to deploy on. Based on the selected device, the c++ code will be compiled for the related processor architecture. You can see the project compiling in the `View > Tool Windows > Gradle Console`.

More information about building and distributing this project in [DISTRIBUTE.md](https://github.com/mapbox/mapbox-gl-native/blob/master/platform/android/DISTRIBUTE.md).

### Additional resources

#### Using the SDK snapshot

Instead of using the latest stable release of the Maps SDK for Android, you can use a "snapshot" or the beta version if there is one available. Our snapshots are built every time a Github pull request adds code to this repository's `master` branch. If you'd like to use a snapshot build, your Android project's gradle file should have -SNAPSHOT appended to the SDK version number. For example `5.2.0-SNAPSHOT` or:

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


#### Symbolicating native crashes

When hitting native crashes you can use ndk-stack to symbolicate crashes.
More information in [this](https://github.com/mapbox/mapbox-gl-native/wiki/Getting-line-numbers-from-an-Android-crash-with-ndk-stack) guide.
