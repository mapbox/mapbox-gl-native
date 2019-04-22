package com.mapbox.mapboxsdk.storage;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.os.AsyncTask;
import android.os.Environment;
import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;

import com.mapbox.mapboxsdk.MapStrictMode;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.log.Logger;
import com.mapbox.mapboxsdk.utils.FileUtils;
import com.mapbox.mapboxsdk.utils.ThreadUtils;

import java.io.File;
import java.lang.ref.WeakReference;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

/**
 * Holds a central reference to the core's DefaultFileSource for as long as
 * there are active mapviews / offline managers
 */
public class FileSource {

  private static final String TAG = "Mbgl-FileSource";
  private static final String MAPBOX_SHARED_PREFERENCE_RESOURCES_CACHE_PATH = "fileSourceResourcesCachePath";
  private static final Lock resourcesCachePathLoaderLock = new ReentrantLock();
  private static final Lock internalCachePathLoaderLock = new ReentrantLock();
  @Nullable
  private static String resourcesCachePath;
  private static String internalCachePath;

  /**
   * This callback allows implementors to transform URLs before they are requested
   * from the internet. This can be used add or remove custom parameters, or reroute
   * certain requests to other servers or endpoints.
   */
  @Keep
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

  /**
   * This callback receives an asynchronous response containing the new path of the
   * resources cache database.
   */
  @Keep
  public interface ResourcesCachePathChangeCallback {

    /**
     * Receives the new database path
     *
     * @param path the path of the current resources cache database
     */
    void onSuccess(String path);

    /**
     * Receives an error message if setting the path was not successful
     *
     * @param message the error message
     */
    void onError(String message);

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
  public static synchronized FileSource getInstance(@NonNull Context context) {
    if (INSTANCE == null) {
      INSTANCE = new FileSource(getResourcesCachePath(context), context.getResources().getAssets());
    }

    return INSTANCE;
  }

  /**
   * Get files directory path for a context.
   *
   * @param context the context to derive the files directory path from
   * @return the files directory path
   */
  @NonNull
  private static String getCachePath(@NonNull Context context) {
    SharedPreferences preferences = context.getSharedPreferences(
      MapboxConstants.MAPBOX_SHARED_PREFERENCES, Context.MODE_PRIVATE);
    String cachePath = preferences.getString(MAPBOX_SHARED_PREFERENCE_RESOURCES_CACHE_PATH, null);

    if (!isPathWritable(cachePath)) {
      // Use default path
      cachePath = getDefaultCachePath(context);

      // Reset stored cache path
      SharedPreferences.Editor editor =
        context.getSharedPreferences(MapboxConstants.MAPBOX_SHARED_PREFERENCES, Context.MODE_PRIVATE).edit();
      editor.remove(MAPBOX_SHARED_PREFERENCE_RESOURCES_CACHE_PATH).apply();
    }

    return cachePath;
  }

  /**
   * Get the default resources cache path depending on the external storage configuration
   *
   * @param context the context to derive the files directory path from
   * @return the default directory path
   */
  @NonNull
  private static String getDefaultCachePath(@NonNull Context context) {
    if (isExternalStorageConfiguration(context) && isExternalStorageReadable()) {
      File externalFilesDir = context.getExternalFilesDir(null);
      if (externalFilesDir != null) {
        return externalFilesDir.getAbsolutePath();
      }
    }
    return context.getFilesDir().getAbsolutePath();
  }

  private static boolean isExternalStorageConfiguration(@NonNull Context context) {
    // Default value
    boolean isExternalStorageConfiguration = MapboxConstants.DEFAULT_SET_STORAGE_EXTERNAL;

    try {
      // Try getting a custom value from the app Manifest
      ApplicationInfo appInfo = context.getPackageManager().getApplicationInfo(context.getPackageName(),
        PackageManager.GET_META_DATA);
      if (appInfo.metaData != null) {
        isExternalStorageConfiguration = appInfo.metaData.getBoolean(
          MapboxConstants.KEY_META_DATA_SET_STORAGE_EXTERNAL,
          MapboxConstants.DEFAULT_SET_STORAGE_EXTERNAL
        );
      }
    } catch (PackageManager.NameNotFoundException exception) {
      Logger.e(TAG, "Failed to read the package metadata: ", exception);
      MapStrictMode.strictModeViolation(exception);
    } catch (Exception exception) {
      Logger.e(TAG, "Failed to read the storage key: ", exception);
      MapStrictMode.strictModeViolation(exception);
    }
    return isExternalStorageConfiguration;
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

    Logger.w(TAG, "External storage was requested but it isn't readable. For API level < 18"
      + " make sure you've requested READ_EXTERNAL_STORAGE or WRITE_EXTERNAL_STORAGE"
      + " permissions in your app Manifest (defaulting to internal storage).");

    return false;
  }

  /**
   * Initializes file directories paths.
   *
   * @param context the context to derive paths from
   */
  @UiThread
  public static void initializeFileDirsPaths(Context context) {
    ThreadUtils.checkThread(TAG);
    lockPathLoaders();
    if (resourcesCachePath == null || internalCachePath == null) {
      new FileDirsPathsTask().execute(context);
    }
  }

  private static class FileDirsPathsTask extends AsyncTask<Context, Void, String[]> {

    @Override
    protected void onCancelled() {
      unlockPathLoaders();
    }

    @NonNull
    @Override
    protected String[] doInBackground(Context... contexts) {
      return new String[] {
        getCachePath(contexts[0]),
        contexts[0].getCacheDir().getAbsolutePath()
      };
    }

    @Override
    protected void onPostExecute(String[] paths) {
      resourcesCachePath = paths[0];
      internalCachePath = paths[1];
      unlockPathLoaders();
    }
  }

  /**
   * Get files directory path for a context.
   *
   * @param context the context to derive the files directory path from
   * @return the files directory path
   */
  @NonNull
  public static String getResourcesCachePath(@NonNull Context context) {
    resourcesCachePathLoaderLock.lock();
    try {
      if (resourcesCachePath == null) {
        resourcesCachePath = getCachePath(context);
      }
      return resourcesCachePath;
    } finally {
      resourcesCachePathLoaderLock.unlock();
    }
  }

  /**
   * Get internal cache path for a context.
   *
   * @param context the context to derive the internal cache path from
   * @return the internal cache path
   */
  public static String getInternalCachePath(@NonNull Context context) {
    internalCachePathLoaderLock.lock();
    try {
      if (internalCachePath == null) {
        internalCachePath = context.getCacheDir().getAbsolutePath();
      }
      return internalCachePath;
    } finally {
      internalCachePathLoaderLock.unlock();
    }
  }

  /**
   * Changes the path of the resources cache database.
   * Note that the external storage setting needs to be activated in the manifest.
   *
   * @param context  the context of the path
   * @param path     the new database path
   * @param callback the callback to obtain the result
   */
  public static void setResourcesCachePath(@NonNull Context context,
                                           @NonNull final String path,
                                           @NonNull ResourcesCachePathChangeCallback callback) {
    final String fileSourceActivatedMessage = "Cannot set path, file source is activated."
      + " Make sure that the map or a resources download is not running.";
    if (getInstance(context).isActivated()) {
      Logger.w(TAG, fileSourceActivatedMessage);
      callback.onError(fileSourceActivatedMessage);
    } else if (path.equals(resourcesCachePath)) {
      // no need to change the path
      callback.onSuccess(path);
    } else {
      final WeakReference<Context> contextWeakReference = new WeakReference<>(context);
      final WeakReference<ResourcesCachePathChangeCallback> callbackWeakReference = new WeakReference<>(callback);
      new FileUtils.CheckFileWritePermissionTask(new FileUtils.OnCheckFileWritePermissionListener() {
        @Override
        public void onWritePermissionGranted() {
          final Context context = contextWeakReference.get();
          final ResourcesCachePathChangeCallback callback = callbackWeakReference.get();

          if (callback == null) {
            Logger.w(TAG, "Lost callback reference.");
            return;
          } else if (context == null) {
            String lostContextMessage = "Lost context reference.";
            Logger.w(TAG, lostContextMessage);
            callback.onError(lostContextMessage);
            return;
          }

          // verify fileSource's activation again after the async task returns
          if (getInstance(context).isActivated()) {
            Logger.w(TAG, fileSourceActivatedMessage);
            callback.onError(fileSourceActivatedMessage);
          } else {
            final SharedPreferences.Editor editor =
              context.getSharedPreferences(MapboxConstants.MAPBOX_SHARED_PREFERENCES, Context.MODE_PRIVATE).edit();
            editor.putString(MAPBOX_SHARED_PREFERENCE_RESOURCES_CACHE_PATH, path);
            editor.apply();
            setResourcesCachePath(context, path);
            callback.onSuccess(path);
          }
        }

        @Override
        public void onError() {
          final ResourcesCachePathChangeCallback callback = callbackWeakReference.get();
          if (callback != null) {
            String message = "Path is not writable: " + path;
            Logger.e(TAG, message);
            callback.onError(message);
          }
        }
      }).execute(new File(path));
    }
  }

  private static void setResourcesCachePath(@NonNull Context context, @NonNull String path) {
    resourcesCachePathLoaderLock.lock();
    resourcesCachePath = path;
    resourcesCachePathLoaderLock.unlock();
    getInstance(context).setResourceCachePath(path);
  }

  private static boolean isPathWritable(String path) {
    if (path == null || path.isEmpty()) {
      return false;
    }
    return new File(path).canWrite();
  }

  private static void lockPathLoaders() {
    internalCachePathLoaderLock.lock();
    resourcesCachePathLoaderLock.lock();
  }

  private static void unlockPathLoaders() {
    resourcesCachePathLoaderLock.unlock();
    internalCachePathLoaderLock.unlock();
  }

  @Keep
  private long nativePtr;

  private FileSource(String cachePath, AssetManager assetManager) {
    initialize(Mapbox.getAccessToken(), cachePath, assetManager);
  }

  @Keep
  public native boolean isActivated();

  @Keep
  public native void activate();

  @Keep
  public native void deactivate();

  @Keep
  public native void setAccessToken(@NonNull String accessToken);

  @NonNull
  @Keep
  public native String getAccessToken();

  @Keep
  public native void setApiBaseUrl(String baseUrl);

  /**
   * Sets a callback for transforming URLs requested from the internet
   * <p>
   * The callback will be executed on the main thread once for every requested URL.
   * </p>
   *
   * @param callback the callback to be invoked or null to reset
   */
  @Keep
  public native void setResourceTransform(final ResourceTransformCallback callback);

  @Keep
  private native void setResourceCachePath(String path);

  @Keep
  private native void initialize(String accessToken, String cachePath, AssetManager assetManager);

  @Override
  @Keep
  protected native void finalize() throws Throwable;

}
