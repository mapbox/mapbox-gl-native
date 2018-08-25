package com.mapbox.mapboxsdk.testapp;

import android.app.Application;
import android.os.StrictMode;
import android.text.TextUtils;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.log.Logger;
import com.mapbox.mapboxsdk.maps.TelemetryDefinition;
import com.mapbox.mapboxsdk.testapp.utils.TimberLogger;
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
    if (!initializeLeakCanary()) {
      return;
    }
    initializeLogger();
    initializeStrictMode();
    initializeMapbox();
  }

  private boolean initializeLeakCanary() {
    if (LeakCanary.isInAnalyzerProcess(this)) {
      // This process is dedicated to LeakCanary for heap analysis.
      // You should not init your app in this process.
      return false;
    }
    LeakCanary.install(this);
    return true;
  }

  private void initializeLogger() {
    Logger.setLoggerDefinition(new TimberLogger());
    if (BuildConfig.DEBUG) {
      Timber.plant(new DebugTree());
    }
  }

  private void initializeStrictMode() {
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
  }

  private void initializeMapbox() {
    String accessToken = TokenUtils.getMapboxAccessToken(getApplicationContext());
    validateAccessToken(accessToken);
    Mapbox.getInstance(getApplicationContext(), accessToken);
    TelemetryDefinition telemetry = Mapbox.getTelemetry();
    if (telemetry == null) {
      throw new IllegalStateException("Telemetry was unavailable during test application start.");
    }
    telemetry.setDebugLoggingEnabled(true);
  }

  private static void validateAccessToken(String accessToken) {
    if (TextUtils.isEmpty(accessToken) || accessToken.equals(DEFAULT_MAPBOX_ACCESS_TOKEN)) {
      Timber.e(ACCESS_TOKEN_NOT_SET_MESSAGE);
    }
  }
}
