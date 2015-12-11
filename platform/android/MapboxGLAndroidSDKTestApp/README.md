# Mapbox GL Test App

## Testing

### Running Espresso tests on a device

This test project comes with all the required Android Testing Support Library dependencies
in the Gradle file. Tests are under the `app/src/androidTest` folder.

Note that before running your tests, you might want to turn off animations on your test device.
It's a known issue that leaving system animations turned on in a test device
(window animation scale, transition animation scale, animator duration scale)
might cause unexpected results, or may lead tests to fail.

To create a new run configuration:
* Click on Run -> Edit Configurations...
* Click on the plus sign and then on "Android Tests"
* Give a name to the configuration, e.g. `TestAppTests`
* Choose the `MapboxGLAndroidSDKTestApp` module
* Choose `android.support.test.runner.AndroidJUnitRunner` as the instrumentation runner
* Click OK to save the new configuration

You can now run this configuration from the main toolbar dropdown menu.

### Running Espresso tests on AWS Device Farm

On a terminal, within `mapbox-gl-native/android/java`,
run the tests (`cC` stands for `connectedCheck`):

```
$ ./gradlew cC -p MapboxGLAndroidSDKTestApp
```

Then:
* Go to your AWS Console and choose Device Farm.
* Create a new project, e.g. `MapboxGLAndroidSDKTestApp`
* On step 1, upload the APK in `mapbox-gl-native/android/java/MapboxGLAndroidSDKTestApp/build/outputs/apk/MapboxGLAndroidSDKTestApp-debug-unaligned.apk`
* On step 2, choose Instrumentation, test filter is `com.mapbox.mapboxgl.testapp.MainActivityTest` and upload the APK in `mapbox-gl-native/android/java/MapboxGLAndroidSDKTestApp/build/outputs/apk/MapboxGLAndroidSDKTestApp-debug-androidTest-unaligned.apk`
* On step 3, choose a device pool. E.g. Top Devices
* On step 4, customize your device state (if needed)
* Finally, confirm the configuration and run the tests.

On Step 2, you can also separate by commas different classes: `com.mapbox.mapboxgl.testapp.MainActivityTest,com.mapbox.mapboxgl.testapp.MainActivityScreenTest`

If you have no tests for your app, or want to test some random user behaviour,
you can just choose "Built-in: Fuzz" in step 2.

### Running pure JUnit tests

These tests run on a local JVM on your development machine and they are extremely fast to run
(as compared to Espresso). These tests are located under `src/test/java`. To run them you switch
to the Unit Tests build variant, then right click the corresponding test class or method
and select "Run ...".

You can also have a run configuration:
* Click on Run -> Edit Configurations...
* Click on "Junit Tests"
* Give a name to the configuration, e.g. `JUnit tests`
* As "Test Kind", choose "All in directory"
* As folder, choose the following folder: `mapbox-gl-native/android/java/MapboxGLAndroidSDKTestApp/src/test/java`
* Click OK to save the new configuration

You can also run the tests from the command line with:

```
$ ./gradlew test --continue -p MapboxGLAndroidSDKTestApp
```

### Running the UI/Application Exerciser Monkey

Similar to the "Built-in: Fuzz" test mentioned above, Android provides
[Monkey](http://developer.android.com/tools/help/monkey.html),
"a program that runs on your emulator or device and generates pseudo-random streams of user events
such as clicks, touches, or gestures, as well as a number of system-level events."

To exercise Monkey on the test app, install the package on the device (e.g. via Android Studio)
and then:

```
$ adb shell monkey -p com.mapbox.mapboxgl.testapp -v 500
```
