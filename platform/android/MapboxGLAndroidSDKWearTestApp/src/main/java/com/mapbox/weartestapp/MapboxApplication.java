package com.mapbox.weartestapp;

import android.app.Application;
import android.os.StrictMode;

import com.mapbox.mapboxsdk.Mapbox;
import com.squareup.leakcanary.LeakCanary;

public class MapboxApplication extends Application {

  @Override
  public void onCreate() {
    super.onCreate();

    // Obtain the API base URL, but don't expect it to exist.
    String apiBaseURL = null;
    int apiBaseURLID = getResources().getIdentifier("mapbox_api_base_url", "string", getPackageName());
    if (apiBaseURLID != 0) {
      apiBaseURL = getString(apiBaseURLID);
    }

    Mapbox.getInstance(getApplicationContext(), getString(R.string.mapbox_access_token), apiBaseURL);

    LeakCanary.install(this);
    StrictMode.setThreadPolicy(new StrictMode.ThreadPolicy.Builder()
      .detectDiskReads()
      .detectDiskWrites()
      .detectNetwork()   // or .detectAll() for all detectable problems
      .penaltyLog()
      .build());
    StrictMode.setVmPolicy(new StrictMode.VmPolicy.Builder()
      .detectLeakedSqlLiteObjects()
      .penaltyLog()
      .penaltyDeath()
      .build());
  }
}
