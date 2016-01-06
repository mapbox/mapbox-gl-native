# Unit tests
Our Unit tests are based on JUnit and are located under `/src/test/java/`. 
We are using plain JUnit to test classes that aren't calling the Android API, 
or are using Android's JUnit extensions to stub/mock Android components.

## Running Unit tests locally
To run Unit tests locally you switch to the Unit Tests build variant, then right click the corresponding test class or method and select "Run ...".

You can also have a run configuration:
* Click on Run -> Edit Configurations...
* Click on "Junit Tests"
* Give a name to the configuration, e.g. `JUnit tests`
* As "Test Kind", choose "All in directory"
* As folder, choose the following folder: `mapbox-gl-native/platforms/android/java/MapboxGLAndroidSDKTestApp/src/test/java`
* Click OK to save the new configuration

You can also run the tests from the command line with:

```
$ ./gradlew test --continue -p MapboxGLAndroidSDKTestApp
```

## Running Unit tests on CI
The Unit tests are executed as part of the build process on our CI and are 
automatically run for each new commit pushed to this repo. If a Unit tests 
fails, this will fail and stop the build.

You can find this gradle command in our [buildscript](https://github.com/mapbox/mapbox-gl-native/blob/master/platform/android/bitrise.yml#L48):

```
$ ./gradlew testReleaseUnitTest --continue
```
