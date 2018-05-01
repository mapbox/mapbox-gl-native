package com.mapbox.mapboxsdk.testrule;

import android.os.Trace;

import org.junit.rules.ExternalResource;
import org.junit.runner.Description;
import org.junit.runners.model.Statement;

/**
 * This rule enables {@link Trace Tracing} for each test. The section name
 * used for the Trace API is the name of the test being run.
 *
 * To enable AndroidTracing on a test simply add this rule like so and it will be enabled/disabled
 * when the platform support for Tracing exists (API Level 18 or higher).
 *
 * <pre>
 * @Rule
 * public EnableTestTracing mEnableTestTracing = new EnableTestTracing();
 * </pre>
 */
public class EnableTestTracing extends ExternalResource {

  private String mTestName;

  @Override
  public Statement apply(Statement base, Description description) {
    mTestName = description.getMethodName();
    return super.apply(base, description);
  }

  @Override
  public void before() {
    if (android.os.Build.VERSION.SDK_INT >= 18) {
      Trace.beginSection(mTestName);
    }
  }

  @Override
  public void after() {
    if (android.os.Build.VERSION.SDK_INT >= 18) {
      Trace.endSection();
    }
  }
}
