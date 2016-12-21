# Core Tests

Core tests run the [unit tests](../../../../test) written for the core rendering component of Mapbox GL.

## Running the tests

To run the tests, connect a device to your computer, or launch a virtual device. Then run

```
$ make run-android-core-test-XXX
```

with `XXX` being the architecture of the device (one of `arm-v5`, `arm-v7`, `arm-v8`, `x86`, `x86-64`,  or `mips`). This command first builds the library, creates a test program, zips up the assets required for the unit tests, deploys it on the device, runs the test and unpacks the results again on your computer.

You can also run individual tests by specifying the name like this:

```
$ make run-android-core-test-XXX-TileCover.*
```

Everything after the last dash is passed as the [`--gtest_filter` argument](https://github.com/google/googletest/blob/master/googletest/docs/AdvancedGuide.md#running-a-subset-of-the-tests) to the Google Test binary.
