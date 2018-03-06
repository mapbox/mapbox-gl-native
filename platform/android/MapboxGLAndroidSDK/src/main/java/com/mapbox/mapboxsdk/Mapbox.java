package com.mapbox.mapboxsdk;

import android.annotation.SuppressLint;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;
import android.text.TextUtils;

import com.mapbox.android.core.location.LocationEngine;
import com.mapbox.android.core.location.LocationEnginePriority;
import com.mapbox.android.core.location.LocationEngineProvider;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.exceptions.MapboxConfigurationException;
import com.mapbox.mapboxsdk.net.ConnectivityReceiver;

/**
 * The entry point to initialize the Mapbox Android SDK.
 * <p>
 * Obtain a reference by calling {@link #getInstance(Context, String)}. Usually this class is configured in
 * Application#onCreate() and is responsible for the active access token, application context, and
 * connectivity state.
 * </p>
 */
@UiThread
public final class Mapbox {

  @SuppressLint("StaticFieldLeak")
  private static Mapbox INSTANCE;
  private Context context;
  private String accessToken;
  private Boolean connected;
  private LocationEngine locationEngine;

  /**
   * Get an instance of Mapbox.
   * <p>
   * This class manages the active access token, application context, and connectivity state.
   * </p>
   *
   * @param context     Android context which holds or is an application context
   * @param accessToken Mapbox access token
   * @return the single instance of Mapbox
   */
  @UiThread
  public static synchronized Mapbox getInstance(@NonNull Context context, @NonNull String accessToken) {
    if (INSTANCE == null) {
      Context appContext = context.getApplicationContext();
      LocationEngineProvider locationEngineProvider = new LocationEngineProvider(context);
      LocationEngine locationEngine = locationEngineProvider.obtainBestLocationEngineAvailable();
      INSTANCE = new Mapbox(appContext, accessToken, locationEngine);
      locationEngine.setPriority(LocationEnginePriority.NO_POWER);

      ConnectivityReceiver.instance(appContext);
    }

    return INSTANCE;
  }

  Mapbox(@NonNull Context context, @NonNull String accessToken, LocationEngine locationEngine) {
    this.context = context;
    this.accessToken = accessToken;
    this.locationEngine = locationEngine;
  }

  /**
   * Access token for this application.
   *
   * @return Mapbox access token
   */
  public static String getAccessToken() {
    validateMapbox();
    validateAccessToken();
    return INSTANCE.accessToken;
  }

  /**
   * Runtime validation of Mapbox creation.
   */
  private static void validateMapbox() throws MapboxConfigurationException {
    if (INSTANCE == null) {
      throw new MapboxConfigurationException();
    }
  }

  /**
   * Runtime validation of access token.
   *
   * @throws MapboxConfigurationException exception thrown when not using a valid accessToken
   */
  private static void validateAccessToken() throws MapboxConfigurationException {
    String accessToken = INSTANCE.accessToken;
    if (TextUtils.isEmpty(accessToken) || (!accessToken.toLowerCase(MapboxConstants.MAPBOX_LOCALE).startsWith("pk.")
      && !accessToken.toLowerCase(MapboxConstants.MAPBOX_LOCALE).startsWith("sk."))) {
      throw new MapboxConfigurationException();
    }
  }

  /**
   * Application context
   *
   * @return the application context
   */
  public static Context getApplicationContext() {
    return INSTANCE.context;
  }

  /**
   * Manually sets the connectivity state of the app. This is useful for apps which control their
   * own connectivity state and want to bypass any checks to the ConnectivityManager.
   *
   * @param connected flag to determine the connectivity state, true for connected, false for
   *                  disconnected, and null for ConnectivityManager to determine.
   */
  public static synchronized void setConnected(Boolean connected) {
    // Connectivity state overridden by app
    INSTANCE.connected = connected;
  }

  /**
   * Determines whether we have an internet connection available. Please do not rely on this
   * method in your apps. This method is used internally by the SDK.
   *
   * @return true if there is an internet connection, false otherwise
   */
  public static synchronized Boolean isConnected() {
    if (INSTANCE.connected != null) {
      // Connectivity state overridden by app
      return INSTANCE.connected;
    }

    ConnectivityManager cm = (ConnectivityManager) INSTANCE.context.getSystemService(Context.CONNECTIVITY_SERVICE);
    NetworkInfo activeNetwork = cm.getActiveNetworkInfo();
    return (activeNetwork != null && activeNetwork.isConnected());
  }

  /**
   * Returns the location engine used by the SDK.
   *
   * @return the location engine configured
   */
  // TODO Do we need to expose this?
  public static LocationEngine getLocationEngine() {
    return INSTANCE.locationEngine;
  }
}
