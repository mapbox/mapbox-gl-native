package com.mapbox.mapboxsdk.testrule;

import android.os.Trace;

import com.mapbox.mapboxsdk.testapp.BuildConfig;

import org.junit.rules.ExternalResource;
import org.junit.runner.Description;
import org.junit.runners.model.Statement;

import static com.mapbox.mapboxsdk.testapp.utils.PerfTestingUtils.getTestFile;
import static org.junit.Assert.assertTrue;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.util.logging.Level;
import java.util.logging.Logger;

public class EnablePostTestDumpsys extends ExternalResource {
  private Logger logger = Logger.getLogger(EnablePostTestDumpsys.class.getName());

  private String mTestName;
  private String mTestClass;

  @Override
  public Statement apply(Statement base, Description description) {
    mTestName = description.getMethodName();
    mTestClass = description.getClassName();
    return super.apply(base, description);
  }

  @Override
  public void before() {
    try {
      Runtime.getRuntime().exec("pm grant com.mapbox.mapboxsdk.testapp android.permission.DUMP");
      Runtime.getRuntime().exec("pm grant com.mapbox.mapboxsdk.testapp android.permission.READ_LOGS");
      Runtime.getRuntime().exec("pm grant com.mapbox.mapboxsdk.testapp android.permission.PACKAGE_USAGE_STATS");
      ProcessBuilder builder = new ProcessBuilder();
      builder.command("dumpsys", "gfxinfo", "--reset",
        // NOTE: Using the android app BuildConfig specifically.
        BuildConfig.APPLICATION_ID);
      Process process = builder.start();
      process.waitFor();
    } catch (Exception exception) {
      logger.log(Level.SEVERE, "Unable to reset dumpsys", exception);
    }
  }

  @Override
  protected void after() {
    if (android.os.Build.VERSION.SDK_INT >= 23) {
      FileWriter fileWriter = null;
      BufferedReader bufferedReader = null;
      try {
        Trace.beginSection("Taking Dumpsys");
        ProcessBuilder processBuilder = new ProcessBuilder();

        // TODO: If less than API level 23 we should remove framestats.
        processBuilder.command("dumpsys", "gfxinfo",
          // NOTE: Using the android app BuildConfig specifically.
          BuildConfig.APPLICATION_ID);

        /*processBuilder.command("dumpsys", "package",
          // NOTE: Using the android app BuildConfig specifically.
          BuildConfig.APPLICATION_ID, "|", "grep", "permission.DUMP");*/
        processBuilder.redirectErrorStream(true);
        Process process = processBuilder.start();
        fileWriter = new FileWriter(getTestFile(mTestClass, mTestName, "gfxinfo.dumpsys"
          + ".log"));
        bufferedReader = new BufferedReader(
          new InputStreamReader(process.getInputStream()));
        String line;
        while ((line = bufferedReader.readLine()) != null) {
          fileWriter.append(line);
          fileWriter.append(System.lineSeparator());
        }
        process.waitFor();
        if (process.exitValue() != 0) {
          throw new Exception("Error while taking dumpsys, exitCode=" +
            process.exitValue());
        }
      } catch (Exception exception) {
        logger.log(Level.SEVERE, "Unable to take a dumpsys", exception);
      } finally {
        if (fileWriter != null) {
          try {
            fileWriter.close();
          } catch (Exception e) {
            e.printStackTrace();
          }
        }
        if (bufferedReader != null) {
          try {
            bufferedReader.close();
          } catch (Exception e) {
            e.printStackTrace();
          }
        }
        Trace.endSection();
      }
    }
  }
}
