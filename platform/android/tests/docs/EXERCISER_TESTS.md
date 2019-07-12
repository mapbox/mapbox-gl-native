#UI/Application Exerciser Tests

UI/application exerciser tests are stress test using random generated users events.

# Monkey Exerciser

The Android SDK provides a test tool called [UI/Application Monkey Exerciser](https://developer.android.com/studio/test/monkey),
"a program that runs on your emulator or device and generates pseudo-random streams of user events
such as clicks, touches, or gestures, as well as a number of system-level events."

## Setup

 - install the qa debug build variant of the test application
 - pin app on screen (this avoid opening statusbar through monkey exerciser)
   - activate with `settings>security>screen pinning`
   - press the recents apps button
   - each app will support a configuration to pin it 
 - run an monkey exerciser with a commands as:
   - `adb shell monkey -p com.mapbox.mapboxsdk.testapp -v --pct-nav 10 --pct-syskeys 2 --pct-anyevent 2 --pct-touch 44 --pct-motion 40 --pct-trackball 2 --monitor-native-crashes 20000`
   - more information on configuration settings in [official docs](https://developer.android.com/studio/test/monkey#command-options-reference)
