# Performance Tests

## What is Systrace?

From the [Android documentation](http://developer.android.com/tools/help/systrace.html):
"Systrace is a tool that will help you analyze the performance of your application by capturing and displaying execution times of your applications processes and other Android system processes."

## What are we using it for?

We’re using Systrace to look for performance issues in our SDK. When we run Systrace while using our app, 
it will collect data from Android then format it into an html file we can view in a web browser. 
When we open the Systrace results, we’re hoping it will be able to help us resolve some of the 
underlying issues associated with our SDK.

## Run systrace locally

Execute following command in your terminal will run Systrace for 10 seconds:

```
python $ANDROID_HOME/platform-tools/systrace/systrace.py --time=10 -o ~/trace.html gfx view res
```

This command will output a file called `trace.html`. 
More information how to interpret the values can be found [here](http://developer.android.com/tools/help/systrace.html).
                                                       
More information about this topic can be found [here](https://codelabs.developers.google.com/codelabs/android-perf-testing/index.html?index=..%2F..%2Fbabbq-2015&viewga=UA-68632703-1#0)

## Automating Systrace with Espresso

The following annotation is being used to isolate the tests and classes that should be included in the performance tests.
                                          
```java
@PerfTest
```

You can optionally define extra rules to gather more data during a performance test:

```java
@Rule
public EnableTestTracing mEnableTestTracing = new EnableTestTracing();

@Rule
public EnablePostTestDumpsys mEnablePostTestDumpsys = new EnablePostTestDumpsys();

@Rule
public EnableLogcatDump mEnableLogcatDump = new EnableLogcatDump();

@Rule
public EnableNetStatsDump mEnableNetStatsDump = new EnableNetStatsDump();
```

## Automating Systrace with MonkeyRunner

An example of such a script can be found [here](https://github.com/googlecodelabs/android-perf-testing/blob/master/run_perf_tests.py).
The script is following this structure:

- Check environment variables
- Define functions
- Clear local data from previous runs
- Find an Android device
- Enable and clear graphics info dumpsys
- Start a systrace thread & test suite thread in parallel
- Wait for both threads to complete
- Download files from device
- Run analysis on downloaded files

## Note
Testing on a device with minimum SDK 6.0 is preferred. 
The tools above work on older versions of Android, but less data will be collected.