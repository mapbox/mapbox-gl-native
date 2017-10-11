#UI Tests
## Running Espresso tests locally

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

## Running Espresso tests on AWS Device Farm

On a terminal, within `mapbox-gl-native/android/java`,
run the tests (`cC` stands for `connectedCheck`):

```
$ ./gradlew -Pmapbox.abis=all cC -p MapboxGLAndroidSDKTestApp
```

Then:
* Go to your AWS Console and choose Device Farm.
* Create a new project, e.g. `MapboxGLAndroidSDKTestApp`
* On step 1, upload the APK in `mapbox-gl-native/platform/android/MapboxGLAndroidSDKTestApp/build/outputs/apk/MapboxGLAndroidSDKTestApp-debug-unaligned.apk`
* On step 2, choose Instrumentation, test filter is `com.mapbox.mapboxgl.testapp.MainActivityTest` and upload the APK in `mapbox-gl-native/platform/android/MapboxGLAndroidSDKTestApp/build/outputs/apk/MapboxGLAndroidSDKTestApp-debug-androidTest-unaligned.apk`
* On step 3, choose a device pool. E.g. Top Devices
* On step 4, customize your device state (if needed)
* Finally, confirm the configuration and run the tests.

On Step 2, you can also separate by commas different classes: `com.mapbox.mapboxgl.testapp.MainActivityTest,com.mapbox.mapboxgl.testapp.MainActivityScreenTest`

If you have no tests for your app, or want to test some random user behavior,
you can just choose "Built-in: Fuzz" in step 2.

### Code coverage
You can generate JaCoCo reports from espresso tests by

- adding this to build.gradle:

 ```java
 buildTypes {
        debug {
           // Run code coverage reports by default on debug builds.
          testCoverageEnabled = true
        }
}
```

- running the gradle task `createMockDebugCoverageReport` when executing tests.

## Running Espresso test automatically on AWS Device Farm
To run tests on AWS device farm you need to execute `./gradlew -Pmapbox.abis=none devicefarmUpload`.
You can configure the different steps in the testapp `build.gradle`.
AWS credentials are found in CircleCI.



