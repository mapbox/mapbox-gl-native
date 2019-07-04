package com.mapbox.mapboxsdk.offline;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import com.mapbox.mapboxsdk.LibraryLoader;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.TelemetryDefinition;
import com.mapbox.mapboxsdk.net.ConnectivityReceiver;
import com.mapbox.mapboxsdk.storage.FileSource;
import com.mapbox.mapboxsdk.utils.FileUtils;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.channels.FileChannel;

/**
 * The offline manager is the main entry point for offline-related functionality.
 * <p>
 * It'll help you list and create offline regions.
 * </p>
 *
 * @see <a href="https://docs.mapbox.com/help/troubleshooting/mobile-offline/">Offline Maps Information/</a>
 */
@UiThread
public class OfflineManager {

  private static final String TAG = "Mbgl - OfflineManager";

  //
  // Static methods
  //

  static {
    LibraryLoader.load();
  }

  // Native peer pointer
  @Keep
  private long nativePtr;

  // Reference to the file source to keep it alive for the
  // lifetime of this object
  private final FileSource fileSource;

  // Makes sure callbacks come back to the main thread
  private final Handler handler = new Handler(Looper.getMainLooper());

  // This object is implemented as a singleton
  @SuppressLint("StaticFieldLeak")
  private static OfflineManager instance;

  // The application context
  private Context context;

  /**
   * This callback receives an asynchronous response containing a list of all
   * OfflineRegion in the database or an error message otherwise.
   */
  @Keep
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
   * OfflineRegion in the database or an error message otherwise.
   */
  @Keep
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

  /**
   * This callback receives an asynchronous response containing a list of all
   * OfflineRegion added to the database during the merge.
   */
  @Keep
  public interface MergeOfflineRegionsCallback {
    /**
     * Receives the list of merged offline regions.
     *
     * @param offlineRegions the offline region array
     */
    void onMerge(OfflineRegion[] offlineRegions);

    /**
     * Receives the error message.
     *
     * @param error the error message
     */
    void onError(String error);
  }

  /*
   * Constructor
   */
  private OfflineManager(Context context) {
    this.context = context.getApplicationContext();
    this.fileSource = FileSource.getInstance(this.context);
    initialize(fileSource);

    // Delete any existing previous ambient cache database
    deleteAmbientDatabase(this.context);
  }

  private void deleteAmbientDatabase(final Context context) {
    final String path = FileSource.getInternalCachePath(context) + File.separator + "mbgl-cache.db";
    FileUtils.deleteFile(path);
  }

  /**
   * Get the single instance of offline manager.
   *
   * @param context the context used to host the offline manager
   * @return the single instance of offline manager
   */
  public static synchronized OfflineManager getInstance(@NonNull Context context) {
    if (instance == null) {
      instance = new OfflineManager(context);
    }

    return instance;
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
    fileSource.activate();
    listOfflineRegions(fileSource, new ListOfflineRegionsCallback() {

      @Override
      public void onList(final OfflineRegion[] offlineRegions) {
        handler.post(new Runnable() {
          @Override
          public void run() {
            fileSource.deactivate();
            callback.onList(offlineRegions);
          }
        });
      }

      @Override
      public void onError(final String error) {
        handler.post(new Runnable() {
          @Override
          public void run() {
            fileSource.deactivate();
            callback.onError(error);
          }
        });
      }
    });
  }

  /**
   * Merge offline regions from a secondary database into the main offline database.
   * <p>
   * When the merge is completed, or fails, the {@link MergeOfflineRegionsCallback} will be invoked on the main thread.
   * The callback reference is <b>strongly kept</b> throughout the process,
   * so it needs to be wrapped in a weak reference or released on the client side if necessary.
   * <p>
   * The secondary database may need to be upgraded to the latest schema.
   * This is done in-place and requires write-access to the provided path.
   * If the app's process doesn't have write-access to the provided path,
   * the file will be copied to the temporary, internal directory for the duration of the merge.
   * <p>
   * Only resources and tiles that belong to a region will be copied over. Identical
   * regions will be flattened into a single new region in the main database.
   * <p>
   * The operation will be aborted and {@link MergeOfflineRegionsCallback#onError(String)} with an appropriate message
   * will be invoked if the merge would result in the offline tile count limit being exceeded.
   * <p>
   * Merged regions may not be in a completed status if the secondary database
   * does not contain all the tiles or resources required by the region definition.
   *
   * @param path     secondary database writable path
   * @param callback completion/error callback
   */
  public void mergeOfflineRegions(@NonNull String path, @NonNull final MergeOfflineRegionsCallback callback) {
    final File src = new File(path);
    new Thread(new Runnable() {
      @Override
      public void run() {
        String errorMessage = null;
        if (src.canWrite()) {
          handler.post(new Runnable() {
            @Override
            public void run() {
              // path writable, merge and update schema in place if necessary
              mergeOfflineDatabaseFiles(src, callback, false);
            }
          });
        } else if (src.canRead()) {
          // path not writable, copy the the file to temp directory
          final File dst = new File(FileSource.getInternalCachePath(context), src.getName());
          try {
            copyTempDatabaseFile(src, dst);
            handler.post(new Runnable() {
              @Override
              public void run() {
                // merge and update schema using the copy
                OfflineManager.this.mergeOfflineDatabaseFiles(dst, callback, true);
              }
            });
          } catch (IOException ex) {
            ex.printStackTrace();
            errorMessage = ex.getMessage();
          }
        } else {
          // path not readable, abort
          errorMessage = "Secondary database needs to be located in a readable path.";
        }

        if (errorMessage != null) {
          final String finalErrorMessage = errorMessage;
          handler.post(new Runnable() {
            @Override
            public void run() {
              callback.onError(finalErrorMessage);
            }
          });
        }
      }
    }).start();
  }

  /**
   * Delete existing database and re-initialize.
   * <p>
   * When the operation is complete or encounters an error, the given callback will be
   * executed on the database thread; it is the responsibility of the SDK bindings
   * to re-execute a user-provided callback on the main thread.
   * </p>
   *
   * @param callback the callback to be invoked when the database was reset or when the operation erred.
   */
  public void resetDatabase(@Nullable final FileSourceCallback callback) {
    fileSource.activate();
    nativeResetDatabase(new FileSourceCallback() {
      @Override
      public void onSuccess() {
        handler.post(new Runnable() {
          @Override
          public void run() {
            fileSource.deactivate();
            if (callback != null) {
              callback.onSuccess();
            }
          }
        });
      }

      @Override
      public void onError(@NonNull final String message) {
        handler.post(new Runnable() {
          @Override
          public void run() {
            fileSource.deactivate();
            if (callback != null) {
              callback.onError(message);
            }
          }
        });
      }
    });
  }

  /**
   * Forces revalidation of the ambient cache.
   * <p>
   * Forces Mapbox GL Native to revalidate resources stored in the ambient
   * cache with the tile server before using them, making sure they
   * are the latest version. This is more efficient than cleaning the
   * cache because if the resource is considered valid after the server
   * lookup, it will not get downloaded again.
   * <p>
   * Resources overlapping with offline regions will not be affected
   * by this call.
   * </p>
   *
   * @param callback the callback to be invoked when the ambient cache was invalidated or when the operation erred.
   */
  public void invalidateAmbientCache(@Nullable final FileSourceCallback callback) {
    fileSource.activate();
    nativeInvalidateAmbientCache(new FileSourceCallback() {
      @Override
      public void onSuccess() {
        handler.post(new Runnable() {
          @Override
          public void run() {
            fileSource.deactivate();
            if (callback != null) {
              callback.onSuccess();
            }
          }
        });
      }

      @Override
      public void onError(@NonNull final String message) {
        handler.post(new Runnable() {
          @Override
          public void run() {
            fileSource.deactivate();
            if (callback != null) {
              callback.onError(message);
            }
          }
        });
      }
    });
  }

  /**
   * Erase resources from the ambient cache, freeing storage space.
   * <p>
   * Erases the ambient cache, freeing resources. This operation can be
   * potentially slow because it will trigger a VACUUM on SQLite,
   * forcing the database to move pages on the filesystem.
   * </p>
   * <p>
   * Resources overlapping with offline regions will not be affected
   * by this call.
   * </p>
   *
   * @param callback the callback to be invoked when the ambient cache was cleared or when the operation erred.
   */
  public void clearAmbientCache(@Nullable final FileSourceCallback callback) {
    fileSource.activate();
    nativeClearAmbientCache(new FileSourceCallback() {
      @Override
      public void onSuccess() {
        handler.post(new Runnable() {
          @Override
          public void run() {
            fileSource.deactivate();
            if (callback != null) {
              callback.onSuccess();
            }
          }
        });
      }

      @Override
      public void onError(@NonNull final String message) {
        handler.post(new Runnable() {
          @Override
          public void run() {
            fileSource.deactivate();
            if (callback != null) {
              callback.onError(message);
            }
          }
        });
      }
    });
  }

  /**
   * Sets the maximum size in bytes for the ambient cache.
   * <p>
   * This call is potentially expensive because it will try
   * to trim the data in case the database is larger than the
   * size defined. The size of offline regions are not affected
   * by this settings, but the ambient cache will always try
   * to not exceed the maximum size defined, taking into account
   * the current size for the offline regions.
   * </p>
   * <p>
   * Note that if you use the SDK's offline functionality, your ability to set the ambient cache size will be limited.
   * Space that offline regions take up detract from the space available for ambient caching, and the ambient cache
   * size does not block offline downloads. For example: if the maximum cache size is set to 50 MB and 40 MB are
   * already used by offline regions, the ambient cache size will effectively be 10 MB.
   * </p>
   * <p>
   * Setting the size to 0 will disable the cache if there is no
   * offline region on the database.
   * </p>
   * <[
   * <p>
   * This method should always be called at the start of an app, before setting the style and loading a map.
   * Otherwise, the map will instantiate with the default cache size of 50 MB.
   * </p>
   *
   * @param size     the maximum size of the ambient cache
   * @param callback the callback to be invoked when the the maximum size has been set or when the operation erred.
   */
  public void setMaximumAmbientCacheSize(long size, @Nullable final FileSourceCallback callback) {
    fileSource.activate();
    nativeSetMaximumAmbientCacheSize(size, new FileSourceCallback() {
      @Override
      public void onSuccess() {
        handler.post(new Runnable() {
          @Override
          public void run() {
            fileSource.deactivate();
            if (callback != null) {
              callback.onSuccess();
            }
          }
        });
      }

      @Override
      public void onError(@NonNull final String message) {
        fileSource.activate();
        handler.post(new Runnable() {
          @Override
          public void run() {
            fileSource.deactivate();
            if (callback != null) {
              callback.onError(message);
            }
          }
        });
      }
    });
  }

  /**
   * This callback receives an asynchronous response indicating if an operation has succeeded or failed.
   */
  @Keep
  public interface FileSourceCallback {

    /**
     * Receives the success of an operation
     */
    void onSuccess();

    /**
     * Receives an error message if an operation was not successful
     *
     * @param message the error message
     */
    void onError(@NonNull String message);

  }

  private static void copyTempDatabaseFile(@NonNull File sourceFile, File destFile) throws IOException {
    if (!destFile.exists() && !destFile.createNewFile()) {
      throw new IOException("Unable to copy database file for merge.");
    }

    FileChannel source = null;
    FileChannel destination = null;

    try {
      source = new FileInputStream(sourceFile).getChannel();
      destination = new FileOutputStream(destFile).getChannel();
      destination.transferFrom(source, 0, source.size());
    } catch (IOException ex) {
      throw new IOException(String.format("Unable to copy database file for merge. %s", ex.getMessage()));
    } finally {
      if (source != null) {
        source.close();
      }
      if (destination != null) {
        destination.close();
      }
    }
  }

  private void mergeOfflineDatabaseFiles(@NonNull final File file, @NonNull final MergeOfflineRegionsCallback callback,
                                         final boolean isTemporaryFile) {
    fileSource.activate();
    mergeOfflineRegions(fileSource, file.getAbsolutePath(), new MergeOfflineRegionsCallback() {
      @Override
      public void onMerge(final OfflineRegion[] offlineRegions) {
        if (isTemporaryFile) {
          file.delete();
        }
        handler.post(new Runnable() {
          @Override
          public void run() {
            fileSource.deactivate();
            callback.onMerge(offlineRegions);
          }
        });
      }

      @Override
      public void onError(final String error) {
        if (isTemporaryFile) {
          file.delete();
        }
        handler.post(new Runnable() {
          @Override
          public void run() {
            fileSource.deactivate();
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
  public void createOfflineRegion(@NonNull OfflineRegionDefinition definition, @NonNull byte[] metadata,
                                  @NonNull final CreateOfflineRegionCallback callback) {
    if (!isValidOfflineRegionDefinition(definition)) {
      callback.onError(
        String.format(context.getString(R.string.mapbox_offline_error_region_definition_invalid),
          definition.getBounds())
      );
      return;
    }

    ConnectivityReceiver.instance(context).activate();
    FileSource.getInstance(context).activate();
    createOfflineRegion(fileSource, definition, metadata, new CreateOfflineRegionCallback() {

      @Override
      public void onCreate(final OfflineRegion offlineRegion) {
        handler.post(new Runnable() {
          @Override
          public void run() {
            ConnectivityReceiver.instance(context).deactivate();
            FileSource.getInstance(context).deactivate();
            callback.onCreate(offlineRegion);
          }
        });
      }

      @Override
      public void onError(final String error) {
        handler.post(new Runnable() {
          @Override
          public void run() {
            ConnectivityReceiver.instance(context).deactivate();
            FileSource.getInstance(context).deactivate();
            callback.onError(error);
          }
        });
      }
    });

    TelemetryDefinition telemetry = Mapbox.getTelemetry();
    if (telemetry != null) {
      LatLngBounds bounds = definition.getBounds();
      telemetry.onCreateOfflineRegion(definition);
    }
  }

  /**
   * Validates if the offline region definition bounds is valid for an offline region download.
   *
   * @param definition the offline region definition
   * @return true if the region fits the world bounds.
   */
  private boolean isValidOfflineRegionDefinition(OfflineRegionDefinition definition) {
    return LatLngBounds.world().contains(definition.getBounds());
  }

  /**
   * Changing or bypassing this limit without permission from Mapbox is prohibited
   * by the Mapbox Terms of Service.
   *
   * @param limit the new tile count limit.
   */
  @Keep
  public native void setOfflineMapboxTileCountLimit(long limit);

  @Keep
  private native void initialize(FileSource fileSource);

  @Override
  @Keep
  protected native void finalize() throws Throwable;

  @Keep
  private native void listOfflineRegions(FileSource fileSource, ListOfflineRegionsCallback callback);

  @Keep
  private native void createOfflineRegion(FileSource fileSource, OfflineRegionDefinition definition,
                                          byte[] metadata, CreateOfflineRegionCallback callback);

  @Keep
  private native void mergeOfflineRegions(FileSource fileSource, String path, MergeOfflineRegionsCallback callback);

  @Keep
  private native void nativeResetDatabase(@Nullable FileSourceCallback callback);

  @Keep
  private native void nativeInvalidateAmbientCache(@Nullable FileSourceCallback callback);

  @Keep
  private native void nativeClearAmbientCache(@Nullable FileSourceCallback callback);

  @Keep
  private native void nativeSetMaximumAmbientCacheSize(long size, @Nullable FileSourceCallback callback);

  /**
   * Insert the provided resource into the ambient cache
   * This method mimics the caching that would take place if the equivalent
   * resource were requested in the process of map rendering.
   * Use this method to pre-warm the cache with resources you know
   * will be requested.
   * <p>
   * This call is asynchronous: the data may not be immediately available
   * for in-progress requests, although subsequent requests should have
   * access to the cached data.
   *
   * @param url            The URL of the resource to insert
   * @param data           Response data to store for this resource. Data is expected to be uncompressed;
   *                       internally, the cache will compress data as necessary.
   * @param modified       Optional "modified" response header, in seconds since 1970, or 0 if not set
   * @param expires        Optional "expires" response header, in seconds since 1970, or 0 if not set
   * @param etag           Optional "entity tag" response header
   * @param mustRevalidate Indicates whether response can be used after it's stale
   */
  @Keep
  public native void putResourceWithUrl(String url, byte[] data, long modified, long expires,
                                        String etag, boolean mustRevalidate);
}
