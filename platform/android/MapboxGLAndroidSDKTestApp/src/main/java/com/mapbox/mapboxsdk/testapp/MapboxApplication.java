package com.mapbox.mapboxsdk.testapp;

import android.app.Application;
import android.os.StrictMode;
import android.text.TextUtils;

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

  private static final String DEFAULT_MAPBOX_ACCESS_TOKEN = "YOUR_MAPBOX_ACCESS_TOKEN_GOES_HERE";
  private static final String ACCESS_TOKEN_NOT_SET_MESSAGE = "In order to run the Test App you need to set a valid "
    + "access token. During development, you can set the MAPBOX_ACCESS_TOKEN environment variable for the SDK to "
    + "automatically include it in the Test App. Otherwise, you can manually include it in the "
    + "res/values/developer-config.xml file in the MapboxGLAndroidSDKTestApp folder.";

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
      Timber.e(ACCESS_TOKEN_NOT_SET_MESSAGE);
    }

    Mapbox.getInstance(getApplicationContext(), mapboxAccessToken);
  }

  private void initializeLogger() {
    if (BuildConfig.DEBUG) {
      Timber.plant(new DebugTree());
    }
  }
}
