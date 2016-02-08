#UI/Application Exerciser Tests

UI/application exerciser tests are stress test using random generated users events.

##Running Locally

The Android SDK provides a test tool called [Monkey](http://developer.android.com/tools/help/monkey.html),
"a program that runs on your emulator or device and generates pseudo-random streams of user events
such as clicks, touches, or gestures, as well as a number of system-level events."

To exercise Monkey on the test app, install the package on the device (e.g. via Android Studio)
and then:

```
$ adb shell monkey -p com.mapbox.mapboxgl.testapp -v 500
```

##Running on AWS Device Farm

Amazon Device farm supports a similar tool called `Built-in Fuzz Test`.
"The built-in fuzz test randomly sends user interface events to devices and then reports results."

More information about [Built-in Fuzz Test](http://docs.aws.amazon.com/devicefarm/latest/developerguide/test-types-built-in-fuzz.html)