package com.mapbox.mapboxsdk;

import android.Manifest;
import android.os.Build;

import static android.support.test.InstrumentationRegistry.getInstrumentation;
import static android.support.test.InstrumentationRegistry.getTargetContext;

public class PermissionGranter {

  private static final String PM_GRANT = "pm grant";
  private static final String SEPARATOR = " ";

  public static void grantLocation() {
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
      getInstrumentation().getUiAutomation().executeShellCommand(
          PM_GRANT + SEPARATOR + getTargetContext().getPackageName() +
          SEPARATOR + Manifest.permission.ACCESS_FINE_LOCATION);
    }
  }
}
