package com.mapbox.mapboxsdk.rules;

import android.os.Trace;
import android.support.annotation.NonNull;
import android.support.test.InstrumentationRegistry;
import android.support.test.uiautomator.UiDevice;

import org.junit.rules.ExternalResource;
import org.junit.runner.Description;
import org.junit.runners.model.Statement;

import java.util.logging.Level;
import java.util.logging.Logger;

import static android.support.test.InstrumentationRegistry.getInstrumentation;
import static com.mapbox.mapboxsdk.utils.TestStorageUtils.buildFileNameFrom;
import static com.mapbox.mapboxsdk.utils.TestStorageUtils.storeResponse;

/**
 * Abstract class to execute dumpsys commands, It will first reset the dumpsys data for the given service.
 */
abstract class AbstractDumpsysRule extends ExternalResource {

  private Logger logger = Logger.getLogger(this.getClass().getName());
  private String packageName;
  private String fileName;

  @Override
  public Statement apply(Statement base, Description description) {
    String testName = description.getMethodName();
    packageName = InstrumentationRegistry.getTargetContext().getPackageName();
    fileName = buildFileNameFrom(testName);
    return super.apply(base, description);
  }

  @Override
  public void before() {
    try {
      UiDevice
        .getInstance(getInstrumentation())
        .executeShellCommand(String.format("dumpsys %s %s --reset", dumpsysService(), packageName));
    } catch (Exception exception) {
      logger.log(Level.SEVERE, "Unable to reset dumpsys", exception);
    }
  }

  @Override
  public void after() {
    try {
      Trace.beginSection("Taking Dumpsys");
      String response = UiDevice
        .getInstance(getInstrumentation())
        .executeShellCommand(String.format("dumpsys %s %s %s", dumpsysService(), packageName, extraOptions()));
      storeResponse(response, dumpsysService(), fileName);
      logger.log(Level.INFO, "Response is: " + response);
    } catch (Exception exception) {
      logger.log(Level.SEVERE, "Unable to take a dumpsys", exception);
    } finally {
      Trace.endSection();
    }
    logger.log(Level.INFO, "Dumpsys taken");
  }

  @NonNull
  protected String extraOptions() {
    return "";
  }

  protected abstract String dumpsysService();
}


