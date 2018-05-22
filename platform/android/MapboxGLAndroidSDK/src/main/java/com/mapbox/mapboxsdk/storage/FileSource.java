package com.mapbox.mapboxsdk.storage;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;

import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import timber.log.Timber;

/**
 * Holds a central reference to the core's DefaultFileSource for as long as
 * there are active mapviews / offline managers
 */
public class FileSource {

  /**
   * This callback allows implementors to transform URLs before they are requested
   * from the internet. This can be used add or remove custom parameters, or reroute
   * certain requests to other servers or endpoints.
   */
  public interface ResourceTransformCallback {

    /**
     * Called whenever a URL needs to be transformed.
     *
     * @param kind the kind of URL to be transformed.
     * @param url  the  URL to be transformed
     * @return a URL that will now be downloaded.
     */
    String onURL(@Resource.Kind int kind, String url);

  }

  // File source instance is kept alive after initialization
  private static FileSource INSTANCE;

  /**
   * Get the single instance of FileSource.
   *
   * @param context the context to derive the cache path from
   * @return the single instance of FileSource
   */
  @UiThread
  public static synchronized FileSource getInstance(Context context) {
    if (INSTANCE == null) {
      String cachePath = getCachePath(context);
      INSTANCE = new FileSource(cachePath, context.getResources().getAssets());
    }

    return INSTANCE;
  }

  /**
   * Get the cache path for a context.
   *
   * @param context the context to derive the cache path from
   * @return the cache path
   */
  public static String getCachePath(Context context) {
    // Default value
    boolean setStorageExternal = MapboxConstants.DEFAULT_SET_STORAGE_EXTERNAL;

    try {
      // Try getting a custom value from the app Manifest
      ApplicationInfo appInfo = context.getPackageManager().getApplicationInfo(context.getPackageName(),
        PackageManager.GET_META_DATA);
      setStorageExternal = appInfo.metaData.getBoolean(
        MapboxConstants.KEY_META_DATA_SET_STORAGE_EXTERNAL,
        MapboxConstants.DEFAULT_SET_STORAGE_EXTERNAL);
    } catch (PackageManager.NameNotFoundException exception) {
      Timber.e(exception, "Failed to read the package metadata: ");
    } catch (Exception exception) {
      Timber.e(exception, "Failed to read the storage key: ");
    }

    String cachePath = null;
    if (setStorageExternal && isExternalStorageReadable()) {
      try {
        // Try getting the external storage path
        cachePath = context.getExternalFilesDir(null).getAbsolutePath();
      } catch (NullPointerException exception) {
        Timber.e(exception, "Failed to obtain the external storage path: ");
      }
    }

    if (cachePath == null) {
      // Default to internal storage
      cachePath = context.getFilesDir().getAbsolutePath();
    }

    return cachePath;
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

  private long nativePtr;

  private FileSource(String cachePath, AssetManager assetManager) {
    initialize(Mapbox.getAccessToken(), cachePath, assetManager);
  }

  public native boolean isActivated();

  public native void activate();

  public native void deactivate();

  public native void setAccessToken(@NonNull String accessToken);

  public native String getAccessToken();

  public native void setApiBaseUrl(String baseUrl);

  /**
   * Sets a callback for transforming URLs requested from the internet
   * <p>
   * The callback will be executed on the main thread once for every requested URL.
   * </p>
   *
   * @param callback the callback to be invoked or null to reset
   */
  public native void setResourceTransform(final ResourceTransformCallback callback);

  private native void initialize(String accessToken, String cachePath, AssetManager assetManager);

  @Override
  protected native void finalize() throws Throwable;

}
