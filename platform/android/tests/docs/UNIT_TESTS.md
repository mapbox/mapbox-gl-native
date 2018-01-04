# Unit tests
**Unit testing is a software development process in which the smallest testable parts of an application, called units, are individually and independently scrutinized for proper operation.**

Our Unit tests are based on JUnit and are located under `/src/test/java/`.
We are using plain JUnit to test classes that aren't calling the Android API,
or are using Android's JUnit extensions to stub/mock Android components.

## Writing unit tests
Unit tests for an Android project are located in the `test` folder:

<img width="348" alt="screen shot 2016-03-16 at 17 24 31" src="https://cloud.githubusercontent.com/assets/2151639/13829301/9ea62418-eb9c-11e5-8ab3-9d6c6bed80a3.png">

To add a test, right click the corresponding package and select create new `Java class`. Name the class appropriately by ending it with `Test` and start adding test methods by applying the `@Test` annotation:

```java
package com.mapbox.mapboxsdk.annotations;

import org.junit.Test;

public class AwesomeSauceTest {

    @Test
    public void simpleTest(){

    }

}

```

you can add methods that are executed before and after each test:

```java
package com.mapbox.mapboxsdk.annotations;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

public class AwesomeSauceTest {

    @Before
    public void beforeEachTest(){

    }

    @Test
    public void simpleTest(){

    }

    @After
    public void afterEachTest(){

    }

}
```

## Running unit tests
You can run a test locally by right clicking and selecting run:

<img width="322" alt="screen shot 2016-03-16 at 17 46 09" src="https://cloud.githubusercontent.com/assets/2151639/13829762/0877af18-eb9f-11e5-87df-6dfb3be64beb.png">

If you like, you can also run with test coverage enabled. This will show you the following dialog:

<img width="465" alt="screen shot 2016-03-16 at 17 58 34" src="https://cloud.githubusercontent.com/assets/2151639/13830064/d097aeca-eba0-11e5-94bd-e38fd1079937.png">

You can also run the tests from the command line with:

```
$ ./gradlew -Pmapbox.abis=none test -p MapboxGLAndroidSDKTestApp
```

## Running Unit tests on CI
The Unit tests are executed as part of the build process on our CI and are
automatically run for each new commit pushed to this repo. If a Unit tests
fails, this will fail and stop the build.

You can find this gradle command in our [buildscript](https://github.com/mapbox/mapbox-gl-native/blob/master/circle.yml#L146-L215):

```
$ ./gradlew -Pmapbox.abis=none testReleaseUnitTest
```


## Code Coverage
Showing code coverage directly in the IDE.
- Switch your Build Variant to the Unit Tests artifact
- Right click a unit test and select `Run test with coverage`
- Select `Add to active suites` //this will create a run configuration
- Edit the run configuration to include/exclude packages in the `Code coverage`-tab.
