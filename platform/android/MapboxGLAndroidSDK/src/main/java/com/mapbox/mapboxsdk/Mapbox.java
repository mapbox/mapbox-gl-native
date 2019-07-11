package com.mapbox.mapboxsdk;

import android.annotation.SuppressLint;
import android.content.Context;
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
  @Nullable
  private String accessToken;
  @Nullable
  private TelemetryDefinition telemetry;
  @Nullable
  private AccountsManager accounts;

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
  @NonNull
  public static synchronized Mapbox getInstance(@NonNull Context context, @Nullable String accessToken) {
    ThreadUtils.init(context);
    ThreadUtils.checkThread(TAG);
    if (INSTANCE == null) {
      Context appContext = context.getApplicationContext();
      FileSource.initializeFileDirsPaths(appContext);
      INSTANCE = new Mapbox(appContext, accessToken);
      if (isAccessTokenValid(accessToken)) {
        initializeTelemetry();
        INSTANCE.accounts = new AccountsManager();
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
   * Get the current active access token for this application.
   *
   * @return Mapbox access token
   */
  @Nullable
  public static String getAccessToken() {
    validateMapbox();
    return INSTANCE.accessToken;
  }

  /**
   * Set the current active accessToken.
   */
  public static void setAccessToken(String accessToken) {
    validateMapbox();
    INSTANCE.accessToken = accessToken;

    // cleanup telemetry which is dependent on an access token
    if (INSTANCE.telemetry != null) {
      INSTANCE.telemetry.disableTelemetrySession();
      INSTANCE.telemetry = null;
    }

    // initialize components dependent on a token
    if (isAccessTokenValid(accessToken)) {
      initializeTelemetry();
      INSTANCE.accounts = new AccountsManager();
    } else {
      INSTANCE.accounts = null;
    }
    FileSource.getInstance(getApplicationContext()).setAccessToken(accessToken);
  }

  /**
   * Returns a SKU token, refreshed if necessary. This method is meant for internal SDK
   * usage only.
   *
   * @return the SKU token
   */
  public static String getSkuToken() {
    if (INSTANCE.accounts == null) {
      throw new MapboxConfigurationException(
        "A valid access token parameter is required when using a Mapbox service."
          + "\nPlease see https://www.mapbox.com/help/create-api-access-token/ to learn how to create one."
          + "\nMore information in this guide https://www.mapbox.com/help/first-steps-android-sdk/#access-tokens."
          + "Currently provided token is: " + INSTANCE.accessToken);
    }
    return INSTANCE.accounts.getSkuToken();
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
    ConnectivityReceiver.instance(INSTANCE.context).setConnected(connected);
  }

  /**
   * Determines whether we have an internet connection available. Please do not rely on this
   * method in your apps. This method is used internally by the SDK.
   *
   * @return true if there is an internet connection, false otherwise
   */
  public static synchronized Boolean isConnected() {
    validateMapbox();
    return ConnectivityReceiver.instance(INSTANCE.context).isConnected();
  }

  /**
   * Initializes telemetry
   */
  private static void initializeTelemetry() {
    try {
      INSTANCE.telemetry = getModuleProvider().obtainTelemetry();
    } catch (Exception exception) {
      String message = "Error occurred while initializing telemetry";
      Logger.e(TAG, message, exception);
      MapStrictMode.strictModeViolation(message, exception);
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
  static boolean isAccessTokenValid(@Nullable String accessToken) {
    if (accessToken == null) {
      return false;
    }

    accessToken = accessToken.trim().toLowerCase(MapboxConstants.MAPBOX_LOCALE);
    return accessToken.length() != 0 && (accessToken.startsWith("pk.") || accessToken.startsWith("sk."));
  }

  /**
   * Internal use. Check if the {@link Mapbox#INSTANCE} is present.
   */
  public static boolean hasInstance() {
    return INSTANCE != null;
  }
}