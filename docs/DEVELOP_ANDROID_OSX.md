# Developing for Android on OS X

Install Oracle JDK 7+, the Android SDK, and Android Studio:

    brew cask install java
    brew install android-sdk
    brew cask install android-studio
    android

In the Android SDK Manager also select and install "Android Support Repository" and "Android Support Library" from "Extras":

![image](https://cloud.githubusercontent.com/assets/98601/9915837/289f398e-5c6e-11e5-9a84-ed4d08d52d1f.png)

By default, the SDK will be installed to `/usr/local/opt/android-sdk`. If you open Android Studio at this point, you may get an error message telling you that it can't find a JVM, it's because you installed a custom Java VM from Oracle. Follow [these instructions](http://tools.android.com/recent/androidstudio1rc3_releasecandidate3released) to start Android Studio. You'll wind up setting these environment variables in your .bash_profile or similar:

    echo "export ANDROID_HOME=`brew --prefix android-sdk`" >> .bash_profile
    echo "export ANDROID_NDK_PATH=`brew --cellar android-ndk`/r10e" >> .bash_profile
    # Replace <path> with something like /Library/Java/JavaVirtualMachines/jdk1.8.0_31.jdk/Contents/Home
    echo "export JAVA_HOME=<path>" >> .bash_profile
    echo "export STUDIO_JDK=$JAVA_HOME" >> .bash_profile

## Setting Mapbox Access Token

_The demo applications use Mapbox vector tiles, which require a Mapbox account and API access token. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/account/apps/)._

gradle will take the value of the `MAPBOX_ACCESS_TOKEN` environ variable and save it to `android/java/MapboxGLAndroidSDKTestApp/src/main/res/raw/token.txt` where the app will read it from.

Run:

    make android
    open -a Android\ Studio

You can then open `android/java` in Android Studio via "Import project (Eclipse ADT, Gradle, etc.)".

**Next: get your app [running on a hardware Android Device](docs/ANDROID_DEVICE.md) or [simulator](docs/ANDROID_SIMULATOR.md)**
