package com.mapbox.mapboxsdk.offline;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.Mapbox;

import java.io.File;

import timber.log.Timber;

/**
 * The offline manager is the main entry point for offline-related functionality.
 * It'll help you list and create offline regions.
 */
public class OfflineManager {

  //
  // Static methods
  //

  static {
    System.loadLibrary("mapbox-gl");
  }

  // Default database name
  private static final String DATABASE_NAME = "mbgl-offline.db";

  /*
   * The maximumCacheSize parameter is a limit applied to non-offline resources only,
   * i.e. resources added to the database for the "ambient use" caching functionality.
   * There is no size limit for offline resources.
   */
  private static final long DEFAULT_MAX_CACHE_SIZE = 50 * 1024 * 1024;

  // Holds the pointer to JNI DefaultFileSource
  private long mDefaultFileSourcePtr = 0;

  // Makes sure callbacks come back to the main thread
  private Handler handler;

  // This object is implemented as a singleton
  private static OfflineManager instance;

  /**
   * This callback receives an asynchronous response containing a list of all
   * {@link OfflineRegion} in the database, or an error message otherwise.
   */
  public interface ListOfflineRegionsCallback {
    /**
     * Receives the list of offline regions.
     *
     * @param offlineRegions the offline region array
     */
    void onList(OfflineRegion[] offlineRegions);

    /**
     * Receives the error message.
     *
     * @param error the error message
     */
    void onError(String error);
  }

  /**
   * This callback receives an asynchronous response containing the newly created
   * {@link OfflineRegion} in the database, or an error message otherwise.
   */
  public interface CreateOfflineRegionCallback {
    /**
     * Receives the newly created offline region.
     *
     * @param offlineRegion the offline region to create
     */
    void onCreate(OfflineRegion offlineRegion);

    /**
     * Receives the error message.
     *
     * @param error the error message to be shown
     */
    void onError(String error);
  }

  /*
   * Constructors
   */
  private OfflineManager(Context context) {
    // TODO: r
    // Get a pointer to the DefaultFileSource instance
    String assetRoot = Mapbox.getDatabasePath(context);
    String cachePath = assetRoot + File.separator + DATABASE_NAME;
    mDefaultFileSourcePtr = createDefaultFileSource(cachePath, assetRoot, DEFAULT_MAX_CACHE_SIZE);
    setAccessToken(mDefaultFileSourcePtr, Mapbox.getDefaultFileSource().getAccessToken());

    // Delete any existing previous ambient cache database
    deleteAmbientDatabase(context);
  }

  private void deleteAmbientDatabase(final Context context) {
    // Delete the file in a separate thread to avoid affecting the UI
    new Thread(new Runnable() {
      @Override
      public void run() {
        try {
          String path = context.getCacheDir().getAbsolutePath() + File.separator + "mbgl-cache.db";
          File file = new File(path);
          if (file.exists()) {
            file.delete();
            Timber.d("Old ambient cache database deleted to save space: " + path);
          }
        } catch (Exception exception) {
          Timber.e("Failed to delete old ambient cache database: ", exception);
        }
      }
    }).start();
  }

  public static synchronized OfflineManager getInstance(Context context) {
    if (instance == null) {
      instance = new OfflineManager(context);
    }

    return instance;
  }

  private Handler getHandler() {
    if (handler == null) {
      handler = new Handler(Looper.getMainLooper());
    }

    return handler;
  }

  /**
   * Retrieve all regions in the offline database.
   * <p>
   * The query will be executed asynchronously and the results passed to the given
   * callback on the main thread.
   * </p>
   *
   * @param callback the callback to be invoked
   */
  public void listOfflineRegions(@NonNull final ListOfflineRegionsCallback callback) {
    listOfflineRegions(mDefaultFileSourcePtr, new ListOfflineRegionsCallback() {
      @Override
      public void onList(final OfflineRegion[] offlineRegions) {
        getHandler().post(new Runnable() {
          @Override
          public void run() {
            callback.onList(offlineRegions);
          }
        });
      }

      @Override
      public void onError(final String error) {
        getHandler().post(new Runnable() {
          @Override
          public void run() {
            callback.onError(error);
          }
        });
      }
    });
  }

  /**
   * Create an offline region in the database.
   * <p>
   * When the initial database queries have completed, the provided callback will be
   * executed on the main thread.
   * </p>
   * <p>
   * Note that the resulting region will be in an inactive download state; to begin
   * downloading resources, call `OfflineRegion.setDownloadState(DownloadState.STATE_ACTIVE)`,
   * optionally registering an `OfflineRegionObserver` beforehand.
   * </p>
   *
   * @param definition the offline region definition
   * @param metadata   the metadata in bytes
   * @param callback   the callback to be invoked
   */
  public void createOfflineRegion(
    @NonNull OfflineRegionDefinition definition,
    @NonNull byte[] metadata,
    @NonNull final CreateOfflineRegionCallback callback) {

    createOfflineRegion(mDefaultFileSourcePtr, definition, metadata, new CreateOfflineRegionCallback() {
      @Override
      public void onCreate(final OfflineRegion offlineRegion) {
        getHandler().post(new Runnable() {
          @Override
          public void run() {
            callback.onCreate(offlineRegion);
          }
        });
      }

      @Override
      public void onError(final String error) {
        getHandler().post(new Runnable() {
          @Override
          public void run() {
            callback.onError(error);
          }
        });
      }
    });
  }

  /*
  * Changing or bypassing this limit without permission from Mapbox is prohibited
  * by the Mapbox Terms of Service.
  */
  public void setOfflineMapboxTileCountLimit(long limit) {
    setOfflineMapboxTileCountLimit(mDefaultFileSourcePtr, limit);
  }


  /*
   * Native methods
   */
  private native long createDefaultFileSource(
    String cachePath, String assetRoot, long maximumCacheSize);

  private native void setAccessToken(long defaultFileSourcePtr, String accessToken);

  private native String getAccessToken(long defaultFileSourcePtr);

  private native void listOfflineRegions(
    long defaultFileSourcePtr, ListOfflineRegionsCallback callback);

  private native void createOfflineRegion(
    long defaultFileSourcePtr, OfflineRegionDefinition definition,
    byte[] metadata, CreateOfflineRegionCallback callback);

  private native void setOfflineMapboxTileCountLimit(
    long defaultFileSourcePtr, long limit);

}
