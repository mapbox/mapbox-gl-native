package com.mapbox.mapboxsdk.offline;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.storage.FileSource;

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


  // Native peer pointer
  private long nativePtr;

  // Reference to the file source to keep it alive for the
  // lifetime of this object
  private final FileSource fileSource;

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
   * Constructor
   */
  private OfflineManager(Context context) {
    this.fileSource = FileSource.getInstance(context);
    initialize(fileSource);

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
    listOfflineRegions(fileSource, new ListOfflineRegionsCallback() {

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

    createOfflineRegion(fileSource, definition, metadata, new CreateOfflineRegionCallback() {

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
  public native void setOfflineMapboxTileCountLimit(long limit);

  private native void initialize(FileSource fileSource);

  @Override
  protected native void finalize() throws Throwable;

  private native void listOfflineRegions(FileSource fileSource, ListOfflineRegionsCallback callback);

  private native void createOfflineRegion(FileSource fileSource, OfflineRegionDefinition definition,
                                          byte[] metadata, CreateOfflineRegionCallback callback);

}
