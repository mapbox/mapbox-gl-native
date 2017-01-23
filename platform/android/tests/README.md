# Mapbox GL Android Test documentation

## Testing
We currently support the following types of testing on the Mapbox Android SDK:

 - [Unit tests](docs/UNIT_TESTS.md) using [JUnit](http://developer.android.com/tools/testing-support-library/index.html#AndroidJUnitRunner)
 - [UI tests](docs/UI_TESTS.md) using [Espresso](http://developer.android.com/tools/testing-support-library/index.html#Espresso)
 - [Exerciser](docs/EXERCISER_TESTS.md) tests using [Monkey](http://developer.android.com/tools/help/monkey.html) or [Build-in Fuzz test](http://docs.aws.amazon.com/devicefarm/latest/developerguide/test-types-built-in-fuzz.html)
 - [Performance tests](docs/PERFORMANCE_TESTS.md) using [Systrace](https://codelabs.developers.google.com/codelabs/android-perf-testing/index.html?index=..%2F..%2Fbabbq-2015&viewga=UA-68632703-1#0)
 - [Core tests](docs/CORE_TESTS.md) using [Google Test](https://github.com/google/googletest) to run the unit tests written for the core rendering component of Mapbox GL