package com.mapbox.mapboxsdk;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.os.Environment;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.support.annotation.NonNull;
import android.text.TextUtils;

import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.exceptions.InvalidAccessTokenException;
import com.mapbox.mapboxsdk.net.ConnectivityReceiver;
import com.mapbox.mapboxsdk.telemetry.MapboxEventManager;
import com.mapbox.mapboxsdk.storage.DefaultFileSource;

import java.io.File;

import timber.log.Timber;

public final class Mapbox {
  private static Mapbox INSTANCE;
  private Context context;
  private Boolean connected;
  private DefaultFileSource fileSource;

  // Default database name
  private static final String DATABASE_NAME = "mbgl-offline.db";

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
  public static synchronized Mapbox getInstance(@NonNull Context context,
                                                @NonNull String accessToken, String apiBaseURL) {
    if (INSTANCE == null) {
      validateAccessToken(accessToken);
      Context appContext = context.getApplicationContext();
      INSTANCE = new Mapbox(appContext, accessToken, apiBaseURL);
      MapboxEventManager.getMapboxEventManager().initialize(appContext, accessToken);
      ConnectivityReceiver.instance(appContext);
    }
    return INSTANCE;
  }

  private Mapbox(@NonNull Context context, @NonNull String accessToken, String apiBaseURL) {
    this.context = context;

    String databasePath = getDatabasePath(context) + File.separator + DATABASE_NAME;
    this.fileSource = new DefaultFileSource(databasePath, context.getPackageCodePath());
    this.fileSource.setAccessToken(accessToken);
    if (apiBaseURL != null && !TextUtils.isEmpty(apiBaseURL)) {
      this.fileSource.setAPIBaseURL(apiBaseURL);
    }
  }

  /**
   * Runtime validation of Access Token.
   *
   * @throws InvalidAccessTokenException exception thrown when not using a valid accessToken
   */
  private static void validateAccessToken(@NonNull String accessToken) throws InvalidAccessTokenException {
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
   * Default file source
   */
  public static DefaultFileSource getDefaultFileSource() {
    return INSTANCE.fileSource;
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

  /**
   * Checks if external storage is available to at least read. In order for this to work, make
   * sure you include &lt;uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE" /&gt;
   * (or WRITE_EXTERNAL_STORAGE) for API level &lt; 18 in your app Manifest.
   * <p>
   * Code from https://developer.android.com/guide/topics/data/data-storage.html#filesExternal
   * </p>
   *
   * @return true if external storage is readable
   */
  public static boolean isExternalStorageReadable() {
    String state = Environment.getExternalStorageState();
    if (Environment.MEDIA_MOUNTED.equals(state) || Environment.MEDIA_MOUNTED_READ_ONLY.equals(state)) {
      return true;
    }

    Timber.w("External storage was requested but it isn't readable. For API level < 18"
      + " make sure you've requested READ_EXTERNAL_STORAGE or WRITE_EXTERNAL_STORAGE"
      + " permissions in your app Manifest (defaulting to internal storage).");

    return false;
  }

  public static String getDatabasePath(Context context) {
    // Default value
    boolean setStorageExternal = MapboxConstants.DEFAULT_SET_STORAGE_EXTERNAL;

    try {
      // Try getting a custom value from the app Manifest
      ApplicationInfo appInfo = context.getPackageManager().getApplicationInfo(
        context.getPackageName(), PackageManager.GET_META_DATA);
      setStorageExternal = appInfo.metaData.getBoolean(
        MapboxConstants.KEY_META_DATA_SET_STORAGE_EXTERNAL,
        MapboxConstants.DEFAULT_SET_STORAGE_EXTERNAL);
    } catch (PackageManager.NameNotFoundException exception) {
      Timber.e("Failed to read the package metadata: ", exception);
    } catch (Exception exception) {
      Timber.e("Failed to read the storage key: ", exception);
    }

    String databasePath = null;
    if (setStorageExternal && isExternalStorageReadable()) {
      try {
        // Try getting the external storage path
        databasePath = context.getExternalFilesDir(null).getAbsolutePath();
      } catch (NullPointerException exception) {
        Timber.e("Failed to obtain the external storage path: ", exception);
      }
    }

    if (databasePath == null) {
      // Default to internal storage
      databasePath = context.getFilesDir().getAbsolutePath();
    }

    return databasePath;
  }
}
