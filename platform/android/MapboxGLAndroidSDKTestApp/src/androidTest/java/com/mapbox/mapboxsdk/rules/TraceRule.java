package com.mapbox.mapboxsdk.rules;

import android.os.Trace;

import org.junit.rules.ExternalResource;
import org.junit.runner.Description;
import org.junit.runners.model.Statement;

/**
 * This rule enables {@link Trace Tracing} for each test. The section name
 * used for the Trace API is the name of the test being run.
 * <p>
 * To enable AndroidTracing on a test simply add this rule like so and it will be enabled/disabled
 * when the platform support for Tracing exists (API Level 18 or higher).
 * <p>
 */
public class TraceRule extends ExternalResource {

  private String testName;

  @Override
  public Statement apply(Statement base, Description description) {
    testName = description.getMethodName();
    return super.apply(base, description);
  }

  @Override
  public void before() {
    Trace.beginSection(testName);
  }

  @Override
  public void after() {
    Trace.endSection();
  }
}
