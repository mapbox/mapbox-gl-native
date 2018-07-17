package com.mapbox.mapboxsdk;

import android.annotation.SuppressLint;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.exceptions.MapboxConfigurationException;
import com.mapbox.mapboxsdk.log.Logger;
import com.mapbox.mapboxsdk.maps.TelemetryDefinition;
import com.mapbox.mapboxsdk.net.ConnectivityReceiver;
import com.mapbox.mapboxsdk.storage.FileSource;
import com.mapbox.mapboxsdk.utils.ThreadUtils;

/**
 * The entry point to initialize the Mapbox Android SDK.
 * <p>
 * Obtain a reference by calling {@link #getInstance(Context, String)}. Usually this class is configured in
 * Application#onCreate() and is responsible for the active access token, application context, and
 * connectivity state.
 * </p>
 */
@UiThread
@SuppressLint("StaticFieldLeak")
public final class Mapbox {

  private static final String TAG = "Mbgl-Mapbox";

  private static ModuleProvider moduleProvider;
  private static Mapbox INSTANCE;

  private Context context;
  private String accessToken;
  private Boolean connected;
  private TelemetryDefinition telemetry;

  /**
   * Get an instance of Mapbox.
   * <p>
   * This class manages the Mapbox access token, application context, and connectivity state.
   * </p>
   *
   * @param context     Android context which holds or is an application context
   * @param accessToken Mapbox access token
   * @return the single instance of Mapbox
   */
  @UiThread
  public static synchronized Mapbox getInstance(@NonNull Context context, @Nullable String accessToken) {
    ThreadUtils.checkThread("Mapbox");
    if (INSTANCE == null) {
      Context appContext = context.getApplicationContext();
      FileSource.initializeFileDirsPaths(appContext);
      INSTANCE = new Mapbox(appContext, accessToken);
      if (isAccessTokenValid(accessToken)) {
        initializeTelemetry();
      }
      ConnectivityReceiver.instance(appContext);
    }
    return INSTANCE;
  }

  Mapbox(@NonNull Context context, @Nullable String accessToken) {
    this.context = context;
    this.accessToken = accessToken;
  }

  /**
   * Access token for this application.
   *
   * @return Mapbox access token
   */
  @Nullable
  public static String getAccessToken() {
    validateMapbox();
    return INSTANCE.accessToken;
  }

  /**
   * Application context
   *
   * @return the application context
   */
  @NonNull
  public static Context getApplicationContext() {
    validateMapbox();
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
    validateMapbox();
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
    validateMapbox();
    if (INSTANCE.connected != null) {
      // Connectivity state overridden by app
      return INSTANCE.connected;
    }

    ConnectivityManager cm = (ConnectivityManager) INSTANCE.context.getSystemService(Context.CONNECTIVITY_SERVICE);
    NetworkInfo activeNetwork = cm.getActiveNetworkInfo();
    return (activeNetwork != null && activeNetwork.isConnected());
  }

  /**
   * Initializes telemetry
   */
  private static void initializeTelemetry() {
    try {
      INSTANCE.telemetry = getModuleProvider().obtainTelemetry();
    } catch (Exception exception) {
      Logger.e(TAG, "Error occured while initializing telemetry", exception);
    }
  }

  /**
   * Get an instance of Telemetry if initialised
   *
   * @return instance of telemetry
   */
  @Nullable
  public static TelemetryDefinition getTelemetry() {
    return INSTANCE.telemetry;
  }

  /**
   * Get the module provider
   *
   * @return moduleProvider
   */
  @NonNull
  public static ModuleProvider getModuleProvider() {
    if (moduleProvider == null) {
      moduleProvider = new ModuleProviderImpl();
    }
    return moduleProvider;
  }

  /**
   * Runtime validation of Mapbox creation.
   */
  private static void validateMapbox() {
    if (INSTANCE == null) {
      throw new MapboxConfigurationException();
    }
  }

  /**
   * Runtime validation of Mapbox access token
   *
   * @param accessToken the access token to validate
   * @return true is valid, false otherwise
   */
  static boolean isAccessTokenValid(String accessToken) {
    if (accessToken == null) {
      return false;
    }

    accessToken = accessToken.trim().toLowerCase(MapboxConstants.MAPBOX_LOCALE);
    return accessToken.length() != 0 && (accessToken.startsWith("pk.") || accessToken.startsWith("sk."));
  }
}