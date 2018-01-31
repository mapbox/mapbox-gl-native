# Distributing Mapbox GL Native for Android

Depending on your use case, you may want to support all or just a subset of [Android ABIs](http://developer.android.com/ndk/guides/abis.html). This document covers building an `.aar` file from the Mapbox Android SDK and building `.so` files for specific ABIs. In normal circumstances an application developer will use [APK splits](https://developer.android.com/studio/build/configure-apk-splits.html) to optimize this at application level.

##### Build types

With a `BUILDTYPE` var you can specify the build type for the `.so` and `.aar` files:

```bash
BUILDTYPE=Debug or BUILDTYPE=Release
```

##### Creating an Android Archive file that supports all ABIs

```sh
BUILDTYPE=Release make apackage
```

This will build native libraries to support following ABIs:
- armeabi
- armeabi-v7a
- arm64-v8a
- x86
- x86_64
- mips

After succesfully finish building the native libraries, gradle will build the MapboxAndroidSDK module and generate an Android Archive file in `MapboxAndroidSDK/build/outputs/aar `. The packaged native libraries can be found in `MapboxAndroidSDK/src/main/jniLibs/<abi>`.

##### Build native libraries for a specific ABI

```sh
make android-lib-%%
```

In the command above you'll need to replace `%%` with an ABI key listed below:

| ABI Key | Android ABI |
| ------- | ----------- |
| arm-v5  | armeabi     |
| arm-v7  | armeabi-v7a |
| arm-v8  | arm64-v8a   |
| x86     | x86         |
| x86-64  | x86_64      |
| mips    | mips        |

For example, to build the arm64-v8a ABI the Makefile target would be:

```sh
make android-lib-arm-v8
```

It's also possible to build multiple ABI (without having to build them all) by running the Makefile multiple times.  For example to build arm64-v8a and x86_64 in the same project the commands would be:

```sh
make android-lib-arm-v8
make android-lib-arm-x86-64
```
