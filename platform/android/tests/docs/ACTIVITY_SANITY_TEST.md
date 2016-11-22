# Activity Sanity Tests

This type of test checks if an Activity doesn't crash at start up. For this project this type of test is  usefull since a lot of core crashes occurs at this point and are ideal to test regressions. The only run for 1 sec each so there are fast and fairly reliable. We are currently generating the test stack from every Activity found in the following package:

> com.mapbox.mapboxsdk.testapp

The generation is done by executing:

> make test-code-android

This command underneath executes the following js file found in:

> node scripts/android-generate-test

and uses the following ejs file as a template.

>platform/android/MapboxGLAndroidSDKTestApp/src/androidTest/java/com/mapbox/mapboxsdk/activity/activity.junit.ejs
