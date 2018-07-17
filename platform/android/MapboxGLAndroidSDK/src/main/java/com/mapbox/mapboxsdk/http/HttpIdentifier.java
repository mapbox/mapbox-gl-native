package com.mapbox.mapboxsdk.http;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.support.annotation.NonNull;
import com.mapbox.mapboxsdk.Mapbox;

public class HttpIdentifier {

  private HttpIdentifier() {
  }

  /**
   * Returns the application identifier, consisting out the package name, version name and version code.
   *
   * @return the application identifier
   */
  public static String getIdentifier() {
    return getIdentifier(Mapbox.getApplicationContext());
  }

  /**
   * Returns the application identifier, consisting out the package name, version name and version code.
   *
   * @param context the context used to retrieve the package manager from
   * @return the application identifier
   */
  private static String getIdentifier(@NonNull Context context) {
    try {
      PackageInfo packageInfo = context.getPackageManager().getPackageInfo(context.getPackageName(), 0);
      return String.format("%s/%s (%s)", context.getPackageName(), packageInfo.versionName, packageInfo.versionCode);
    } catch (Exception exception) {
      return "";
    }
  }
}
