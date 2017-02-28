package com.mapbox.mapboxsdk;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.support.annotation.NonNull;
import android.text.TextUtils;

import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.exceptions.InvalidAccessTokenException;
import com.mapbox.mapboxsdk.location.LocationSource;
import com.mapbox.mapboxsdk.net.ConnectivityReceiver;
import com.mapbox.services.android.telemetry.MapboxTelemetry;
import com.mapbox.services.android.telemetry.location.LocationEngine;
import com.mapbox.services.android.telemetry.location.LocationEnginePriority;

public final class Mapbox {

  private static Mapbox INSTANCE;
  private Context context;
  private String accessToken;
  private Boolean connected;

  /**
   * Get an instance of Mapbox.
   * <p>
   * This class manages the active access token, application context and connectivity state.
   * </p>
   *
   * @param context     Android context which holds or is an application context
   * @param accessToken Mapbox access token
   * @return the single instance of Mapbox
   */
  public static synchronized Mapbox getInstance(@NonNull Context context, @NonNull String accessToken) {
    if (INSTANCE == null) {
      Context appContext = context.getApplicationContext();
      INSTANCE = new Mapbox(appContext, accessToken);
      LocationEngine locationEngine = new LocationSource(appContext);
      locationEngine.setPriority(LocationEnginePriority.NO_POWER);
      MapboxTelemetry.getInstance().initialize(appContext, accessToken, locationEngine);
      ConnectivityReceiver.instance(appContext);
    }
    return INSTANCE;
  }

  Mapbox(@NonNull Context context, @NonNull String accessToken) {
    this.context = context;
    this.accessToken = accessToken;
  }

  /**
   * Access Token for this application.
   *
   * @return Mapbox Access Token
   */
  public static String getAccessToken() {
    validateAccessToken();
    return INSTANCE.accessToken;
  }

  /**
   * Runtime validation of Access Token.
   *
   * @throws InvalidAccessTokenException exception thrown when not using a valid accessToken
   */
  private static void validateAccessToken() throws InvalidAccessTokenException {
    String accessToken = INSTANCE.accessToken;
    if (TextUtils.isEmpty(accessToken) || (!accessToken.toLowerCase(MapboxConstants.MAPBOX_LOCALE).startsWith("pk.")
      && !accessToken.toLowerCase(MapboxConstants.MAPBOX_LOCALE).startsWith("sk."))) {
      throw new InvalidAccessTokenException();
    }
  }

  /**
   * Application context
   */
  public static Context getApplicationContext() {
    return INSTANCE.context;
  }

  /**
   * Manually sets the connectivity state of the app. This is useful for apps that control their
   * own connectivity state and want to bypass any checks to the ConnectivityManager.
   *
   * @param connected flag to determine the connectivity state, true for connected, false for
   *                  disconnected, null for ConnectivityManager to determine.
   */
  public static synchronized void setConnected(Boolean connected) {
    // Connectivity state overridden by app
    INSTANCE.connected = connected;
  }

  /**
   * Determines whether we have an Internet connection available. Please do not rely on this
   * method in your apps, this method is used internally by the SDK.
   *
   * @return true if there is an Internet connection, false otherwise
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
}
