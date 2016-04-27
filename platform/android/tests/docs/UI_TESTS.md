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
To automatically execute Espresso tests as part of our CI build, we have created a Python script called [`devicefarm.py`](https://github.com/mapbox/mapbox-gl-native/blob/master/platform/android/tests/scripts/devicefarm.py).

This script is responsible for:
 - uploading an APK + test APK
 - scheduling tests
 - exiting with a return code
  - 0 -> all tests have passed
  - 1 otherwise

### Requirements

  * [Boto 3](http://boto3.readthedocs.org)
  * [Requests](http://www.python-requests.org)

### Running the script

  A sample run would be as follows:

  ```
  $ python devicefarm.py \
  	--project-arn "arn:aws:devicefarm:us-west-2:XXXXX" \
  	--device-pool-arn "arn:aws:devicefarm:us-west-2::devicepool:YYYYY" \
  	--app-apk-path app/build/outputs/apk/app-debug-unaligned.apk \
  	--test-apk-path app/build/outputs/apk/app-debug-androidTest-unaligned.apk
  ```

  Where you need to insert your actual project and device ARNs. We follow Boto 3
  conventions to to [set up the AWS credentials](https://github.com/boto/boto3#quick-start).

  You can build the `app-debug-androidTest-unaligned.apk` package with Gradle:

  ```
  ./gradlew assembleAndroidTest
  ```

  To run tests locally, you can use `./gradlew assemble` to build the app APK, and
  `./gradlew test --continue` to run unit tests. Finally, `./gradlew connectedAndroidTest`
  will run the Espresso tests in a local device.

  A sample output would be as follows:

  ```
  Starting upload: ANDROID_APP
  Uploading: ../app/build/outputs/apk/app-debug-unaligned.apk
  Checking if the upload succeeded.
  Upload not ready (status is INITIALIZED), waiting for 5 seconds.
  Starting upload: INSTRUMENTATION_TEST_PACKAGE
  Uploading: ../app/build/outputs/apk/app-debug-androidTest-unaligned.apk
  Checking if the upload succeeded.
  Upload not ready (status is INITIALIZED), waiting for 5 seconds.
  Scheduling a run.
  Checking if the run succeeded.
  Run not completed (status is SCHEDULING), waiting for 60 seconds.
  Run not completed (status is RUNNING), waiting for 60 seconds.
  Run not completed (status is RUNNING), waiting for 60 seconds.
  Run not completed (status is RUNNING), waiting for 60 seconds.
  Run not completed (status is RUNNING), waiting for 60 seconds.
  Run not completed (status is RUNNING), waiting for 60 seconds.
  Run completed: PASSED
  ```

### Available commands

  You can use the `--help` command to get a list of all available options:

  ```
  $ python devicefarm.py  --help
  usage: Device Farm Runner [-h] [--project-arn PROJECT_ARN]
                            [--device-pool-arn DEVICE_POOL_ARN]
                            [--app-apk-path APP_APK_PATH]
                            [--test-apk-path TEST_APK_PATH]

  Runs the Espresso tests on AWS Device Farm.

  optional arguments:
    -h, --help            show this help message and exit
    --project-arn PROJECT_ARN
                          The project ARN (Amazon Resource Name) (default: None)
    --device-pool-arn DEVICE_POOL_ARN
                          The device pool ARN (Amazon Resource Name) (default:
                          None)
    --app-apk-path APP_APK_PATH
                          Path to the app APK (default: None)
    --test-apk-path TEST_APK_PATH
                          Path to the tests APK (default: None)
  ```



