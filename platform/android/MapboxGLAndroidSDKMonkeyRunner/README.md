# Monkey Exerciser

This module is used to run [UI/Application Monkey Exerciser](https://developer.android.com/studio/test/monkey) on the Mapbox Maps SDK for Android:

> The Monkey is a program that runs on your emulator or device and generates pseudo-random streams of user events such as clicks, touches, or gestures, as well as a number of system-level events. You can use the Monkey to stress-test applications that you are developing, in a random yet repeatable manner.

## Setup

 - install app from this module
 - pin app on screen (this avoid opening statusbar through monkey exerciser)
   - activate with `settings>security>screen pinning`
   - press the recents apps button
   - each app will support a configuration to pin it 
 - run an monkey exerciser commands as:
   - `adb shell monkey -p com.mapbox.mapboxsdk.testapp.monkey -v --pct-nav 0 --pct-syskeys 0 --pct-anyevent 0 --pct-touch 20 --pct-motion 80 --pct-trackball 0 --monitor-native-crashes 20000`
   - `adb shell monkey -p com.mapbox.mapboxsdk.testapp.monkey -v --pct-nav 10 --pct-syskeys 2 --pct-anyevent 2 --pct-touch 44 --pct-motion 40 --pct-trackball 2 --monitor-native-crashes 20000`
