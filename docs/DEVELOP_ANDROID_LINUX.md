# Developing for Android on Linux

Install a build dependencies:

    apt-get install -y make git build-essential automake \
    libtool make cmake pkg-config lib32stdc++6 lib32z1

Install [Oracle JDK 7 (requires license agreement)](http://www.oracle.com/technetwork/java/javase/downloads/jdk7-downloads-1880260.html)

    export JAVA_HOME="/dir/to/jdk1.7.0_71"

Install the Android SDK. We recommend doing this by way of [Android Studio](https://developer.android.com/sdk/installing/studio.html).

    export ANDROID_HOME="/dir/to/android-sdk-linux"

In the Android SDK Manager also select and install "Android Support Repository" and "Android Support Library" from "Extras":

![image](https://cloud.githubusercontent.com/assets/98601/9915837/289f398e-5c6e-11e5-9a84-ed4d08d52d1f.png)

## Setting Mapbox Access Token

_The demo applications use Mapbox vector tiles, which require a Mapbox account and API access token. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/account/apps/)._

gradle will take the value of the `MAPBOX_ACCESS_TOKEN` environ variable and save it to `android/java/MapboxGLAndroidSDKTestApp/src/main/res/raw/token.txt` where the app will read it from.

## Building

Run:

    make android

You can then open `android/java` in Android Studio via "Import project (Eclipse ADT, Gradle, etc.)".

**Next: get your app [running on a hardware Android Device](docs/ANDROID_DEVICE.md) or [simulator](docs/ANDROID_SIMULATOR.md)**
