package com.mapbox.mapboxsdk.testrule;

import com.mapbox.mapboxsdk.testapp.BuildConfig;

import org.junit.rules.ExternalResource;
import org.junit.runner.Description;
import org.junit.runners.model.Statement;

/**
 * This rule enables pulling an atrace file that can be run through systrace.py
 *
 * This JUnit rule requires the test be running on a device that has been rooted. If you don't
 * have a device with root you should prefer the systrace be pulled at the system level.
 *
 * To enable, add this rule to your test class.
 *
 * <pre>
 * @Rule
 * public EnablePerTestTraceFile mEnablePerTestTraceFile = new EnablePerTestTraceFile();
 * </pre>
 */
public class EnablePerTestTraceFile extends ExternalResource {

  private String mTestName;
  private boolean aTraceInUse = false;

  @Override
  public Statement apply(Statement base, Description description) {
    mTestName = description.getMethodName();
    return super.apply(base, description);
  }

  @Override
  public void before() {
    try {
      ProcessBuilder builder = new ProcessBuilder();
      builder.command("atrace", "--async_start", "-a",
        // NOTE: Using the android app BuildConfig specifically.
        BuildConfig.APPLICATION_ID);
      Process process = builder.start();
      process.waitFor();
      if (process.exitValue() == 0) {
        aTraceInUse = true;
      }
    } catch (Exception ignored) {
      // Not much we can do if atrace isn't enabled on the device.
    }
  }

  @Override
  public void after() {
    if (aTraceInUse) {
      try {
        ProcessBuilder builder = new ProcessBuilder();
        builder.command("atrace", "--async_stop", "-a",
          // NOTE: Using the android app BuildConfig specifically.
          BuildConfig.APPLICATION_ID);
        Process process = builder.start();
        process.waitFor();
      } catch (Exception ignored) {
        // Not much we can do if atrace isn't enabled on the device.
      }
    }
  }
}

