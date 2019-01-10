package com.mapbox.mapboxsdk.storage;

import android.annotation.TargetApi;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;

import com.mapbox.mapboxsdk.MapStrictMode;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.offline.OfflineManager;
import com.mapbox.mapboxsdk.offline.OfflineRegion;
import com.mapbox.mapboxsdk.utils.FileUtils;
import com.mapbox.mapboxsdk.utils.ThreadUtils;

import java.io.File;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import com.mapbox.mapboxsdk.log.Logger;

/**
 * Holds a central reference to the core's DefaultFileSource for as long as
 * there are active mapviews / offline managers
 */
public class FileSource {

  private static final String TAG = "Mbgl-FileSource";
  private static final String MAPBOX_SHARED_PREFERENCES = "MapboxSharedPreferences";
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
   * This callback receives an asynchronous response containing a list of possible
   * paths for the resources cache database.
   */
  @Keep
  public interface PossibleResourcesCachePathsCallback {

    /**
     * Receives the list of possible paths
     *
     * @param possiblePaths the paths list
     */
    void onReceive(@NonNull List<String> possiblePaths);

  }

  /**
   * This callback receives an asynchronous response containing the migration path
   * and the list of the successfully migrated offline regions.
   */
  @Keep
  public interface MigrateResourcesCachePathCallback {

    /**
     * Receives the migration path and the migrated offline regions
     *
     * @param migrationPath  the path of the current resources cache database
     * @param offlineRegions the list if the migrated offline regions
     */
    void onMigrate(String migrationPath, OfflineRegion[] offlineRegions);

    /**
     * Receives an error message if the migration was not successful
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
    String cachePath = null;
    if (isExternalStorageConfiguration(context) && isExternalStorageReadable()) {
      try {
        // Try getting the external storage path
        SharedPreferences preferences = context.getSharedPreferences(MAPBOX_SHARED_PREFERENCES, Context.MODE_PRIVATE);
        String defaultExternalStoragePath = context.getExternalFilesDir(null).getAbsolutePath();
        cachePath = preferences.getString(MAPBOX_SHARED_PREFERENCE_RESOURCES_CACHE_PATH, defaultExternalStoragePath);
        if (!isValidPath(context, cachePath)) {
          cachePath = null;
        }
      } catch (NullPointerException exception) {
        Logger.e(TAG, "Failed to obtain the external storage path: ", exception);
        MapStrictMode.strictModeViolation(exception);
      }
    }

    if (cachePath == null) {
      // Default to internal storage
      cachePath = context.getFilesDir().getAbsolutePath();

      // Reset cache path
      SharedPreferences.Editor editor =
          context.getSharedPreferences(MAPBOX_SHARED_PREFERENCES, Context.MODE_PRIVATE).edit();
      editor.remove(MAPBOX_SHARED_PREFERENCE_RESOURCES_CACHE_PATH).apply();
    }

    return cachePath;
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
    ThreadUtils.checkThread("FileSource");
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
      return new String[]{
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
  @Nullable
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
   * Changes the path of the resources cache database. Possible paths can be retrieved with
   * {@link #requestPossibleResourcesCachePaths(Context, PossibleResourcesCachePathsCallback)}.
   * Note that the external storage setting needs to be activated in the manifest.
   *
   * @param context       the context for the migration process
   * @param migrationPath the migration path
   * @param callback      the callback to obtain the result of the migration
   */
  public static void migrateToResourcesCachePath(@NonNull Context context,
                                                 @NonNull final String migrationPath,
                                                 @NonNull final MigrateResourcesCachePathCallback callback) {
    final WeakReference<Context> contextWeakReference = new WeakReference<>(context);
    new Thread(new Runnable() {
      @Override
      public void run() {
        final Context context = contextWeakReference.get();
        final String message;
        if (context != null) {
          if (resourcesCachePath != null) {
            if (!isExternalStorageConfiguration(context)) {
              message = "External storage setting is not enabled!";
            } else if (resourcesCachePath.equals(migrationPath)) {
              message = "Migration not needed";
            } else if (!isExternalStorageReadable()) {
              message = "External storage is not readable!";
            } else if (!isValidPath(context, migrationPath)) {
              message = "Migration path is not valid: " + migrationPath;
            } else {
              message = null;
              new Handler(Looper.getMainLooper()).post(new Runnable() {
                @Override
                public void run() {
                  startMigration(context, migrationPath, callback);
                }
              });
            }
          } else {
            message = "Resources cache path was not initialized.";
          }
        } else {
          message = "Context is null";
        }

        if (message != null) {
          Logger.w(TAG, message);
          new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
              callback.onError(message);
            }
          });
        }
      }
    }).start();
  }

  private static void startMigration(@NonNull final Context context,
                                     @NonNull final String migrationPath,
                                     @NonNull final MigrateResourcesCachePathCallback callback) {

    final String oldPath = resourcesCachePath;
    final String oldDatabasePath = oldPath + File.separator + "mbgl-offline.db";
    resourcesCachePathLoaderLock.lock();
    resourcesCachePath = migrationPath;
    reinitializeOfflineManager(context);
    resourcesCachePathLoaderLock.unlock();

    // merge
    final OfflineManager offlineManager = OfflineManager.getInstance(context);
    offlineManager.mergeOfflineRegions(oldDatabasePath, new OfflineManager.MergeOfflineRegionsCallback() {
      @Override
      public void onMerge(OfflineRegion[] offlineRegions) {
        FileUtils.deleteFile(oldDatabasePath);

        new Thread(new Runnable() {
          @Override
          public void run() {
            SharedPreferences.Editor editor =
                context.getSharedPreferences(MAPBOX_SHARED_PREFERENCES, Context.MODE_PRIVATE).edit();
            editor.putString(MAPBOX_SHARED_PREFERENCE_RESOURCES_CACHE_PATH, migrationPath).apply();
          }
        }).start();

        callback.onMigrate(migrationPath, offlineRegions);
      }

      @Override
      public void onError(String error) {
        resourcesCachePathLoaderLock.lock();
        resourcesCachePath = oldPath;
        reinitializeOfflineManager(context);
        resourcesCachePathLoaderLock.unlock();

        callback.onError(error);
      }
    });
  }

  private static boolean isValidPath(@NonNull Context context, String resourcesCachePath) {
    if (resourcesCachePath == null || resourcesCachePath.isEmpty()) {
      return false;
    }
    for (String externalStoragePath : getPossibleResourcesCachePaths(context)) {
      if (resourcesCachePath.equals(externalStoragePath)) {
        return true;
      }
    }
    return false;
  }

  /**
   * Get possible paths for the resource cache database migration.
   * See {@link #migrateToResourcesCachePath(Context, String, MigrateResourcesCachePathCallback)}.
   *
   * @param context  the context to retrieve the paths from
   * @param callback the callback for the response
   */
  public static void requestPossibleResourcesCachePaths(@NonNull final Context context,
                                                        @NonNull final PossibleResourcesCachePathsCallback callback) {
    new Thread(new Runnable() {
      @Override
      public void run() {
        callback.onReceive(getPossibleResourcesCachePaths(context));
      }
    }).start();
  }

  private static List<String> getPossibleResourcesCachePaths(@NonNull final Context context) {
    if (!isExternalStorageConfiguration(context)) {
      Logger.w(TAG, "External storage setting is not enabled!");
      return Collections.emptyList();
    }
    return obtainExternalFilesPaths(context);
  }

  private static List<String> obtainExternalFilesPaths(@NonNull Context context) {
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
      return obtainExternalFilesPathsKitKat(context);
    }
    return obtainExternalFilesPathsLegacy(context);
  }

  private static List<String> obtainExternalFilesPathsLegacy(@NonNull Context context) {
    final String postFix = File.separator + "Android" + File.separator + "data" + File.separator
        + context.getPackageName() + File.separator + "files";
    final List<String> paths = new ArrayList<>();
    final String externalStorage = System.getenv("EXTERNAL_STORAGE");
    final String secondaryStorage = System.getenv("SECONDARY_STORAGE");
    if (externalStorage != null) {
      paths.add(externalStorage + postFix);
    }
    if (secondaryStorage != null) {
      final String[] secPaths = secondaryStorage.split(":");
      for (String path : secPaths) {
        paths.add(path + postFix);
      }
    }
    return paths;
  }

  @TargetApi(Build.VERSION_CODES.KITKAT)
  private static List<String> obtainExternalFilesPathsKitKat(@NonNull Context context) {
    final List<String> paths = new ArrayList<>();
    final File[] extDirs = context.getExternalFilesDirs(null);
    for (File dir : extDirs) {
      paths.add(dir.getAbsolutePath());
    }
    return paths;
  }

  private static void reinitializeOfflineManager(@NonNull Context context) {
    final FileSource fileSource = FileSource.getInstance(context);
    fileSource.initialize(Mapbox.getAccessToken(), resourcesCachePath, context.getResources().getAssets());
    OfflineManager.clear();
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
  private native void initialize(String accessToken, String cachePath, AssetManager assetManager);

  @Override
  @Keep
  protected native void finalize() throws Throwable;

}
