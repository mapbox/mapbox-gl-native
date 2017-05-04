package com.mapbox.mapboxsdk.testapp;

import android.app.Application;
import android.os.StrictMode;
import android.text.TextUtils;
import android.util.Log;

import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.testapp.utils.TokenUtils;
import com.squareup.leakcanary.LeakCanary;

import timber.log.Timber;

import static timber.log.Timber.DebugTree;

/**
 * Application class of the test application.
 * <p>
 * Initialises components as LeakCanary, Strictmode, Timber and Mapbox
 * </p>
 */
public class MapboxApplication extends Application {

  private static final String LOG_TAG = MapboxApplication.class.getSimpleName();
  private static final String DEFAULT_MAPBOX_ACCESS_TOKEN = "YOUR_MAPBOX_ACCESS_TOKEN_GOES_HERE";

  @Override
  public void onCreate() {
    super.onCreate();

    if (LeakCanary.isInAnalyzerProcess(this)) {
      // This process is dedicated to LeakCanary for heap analysis.
      // You should not init your app in this process.
      return;
    }
    LeakCanary.install(this);

    initializeLogger();

    StrictMode.setThreadPolicy(new StrictMode.ThreadPolicy.Builder()
      .detectDiskReads()
      .detectDiskWrites()
      .detectNetwork()
      .penaltyLog()
      .build());
    StrictMode.setVmPolicy(new StrictMode.VmPolicy.Builder()
      .detectLeakedSqlLiteObjects()
      .penaltyLog()
      .penaltyDeath()
      .build());

    String mapboxAccessToken = TokenUtils.getMapboxAccessToken(getApplicationContext());
    if (TextUtils.isEmpty(mapboxAccessToken) || mapboxAccessToken.equals(DEFAULT_MAPBOX_ACCESS_TOKEN)) {
      Log.w(LOG_TAG, "Warning: access token isn't set.");
    }

    Mapbox.getInstance(getApplicationContext(), mapboxAccessToken);
  }

  private void initializeLogger() {
    if (BuildConfig.DEBUG) {
      Timber.plant(new DebugTree());
    }
  }
}
