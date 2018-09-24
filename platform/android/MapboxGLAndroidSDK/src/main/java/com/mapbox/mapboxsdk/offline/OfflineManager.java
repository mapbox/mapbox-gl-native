package com.mapbox.mapboxsdk.offline;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.Keep;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.LibraryLoader;
import com.mapbox.mapboxsdk.MapStrictMode;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.log.Logger;
import com.mapbox.mapboxsdk.net.ConnectivityReceiver;
import com.mapbox.mapboxsdk.storage.FileSource;
import com.mapbox.mapboxsdk.utils.FileUtils;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.nio.channels.FileChannel;

/**
 * The offline manager is the main entry point for offline-related functionality.
 * It'll help you list and create offline regions.
 */
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
  private Handler handler;

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
    // Delete the file in a separate thread to avoid affecting the UI
    new Thread(new Runnable() {
      @Override
      public void run() {
        try {
          String path = FileSource.getInternalCachePath(context) + File.separator + "mbgl-cache.db";
          File file = new File(path);
          if (file.exists()) {
            file.delete();
            Logger.d(TAG, String.format("Old ambient cache database deleted to save space: %s", path));
          }
        } catch (Exception exception) {
          Logger.e(TAG, "Failed to delete old ambient cache database: ", exception);
          MapStrictMode.strictModeViolation(exception);
        }
      }
    }).start();
  }

  /**
   * Get the single instance of offline manager.
   *
   * @param context the context used to host the offline manager
   * @return the single instance of offline manager
   */
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
    fileSource.activate();
    listOfflineRegions(fileSource, new ListOfflineRegionsCallback() {

      @Override
      public void onList(final OfflineRegion[] offlineRegions) {
        getHandler().post(new Runnable() {
          @Override
          public void run() {
            fileSource.deactivate();
            callback.onList(offlineRegions);
          }
        });
      }

      @Override
      public void onError(final String error) {
        getHandler().post(new Runnable() {
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
    File src = new File(path);
    new FileUtils.CheckFileReadPermissionTask(new FileUtils.OnCheckFileReadPermissionListener() {
      @Override
      public void onReadPermissionGranted() {
        new FileUtils.CheckFileWritePermissionTask(new FileUtils.OnCheckFileWritePermissionListener() {
          @Override
          public void onWritePermissionGranted() {
            // path writable, merge and update schema in place if necessary
            mergeOfflineDatabaseFiles(src, callback, false);
          }

          @Override
          public void onError() {
            // path not writable, copy the the file to temp directory, then merge and update schema on a copy if
            // necessary
            File dst = new File(FileSource.getInternalCachePath(context), src.getName());
            new CopyTempDatabaseFileTask(OfflineManager.this, callback).execute(src, dst);
          }
        }).execute(src);
      }

      @Override
      public void onError() {
        // path not readable, abort
        callback.onError("Secondary database needs to be located in a readable path.");
      }
    }).execute(src);
  }

  private static final class CopyTempDatabaseFileTask extends AsyncTask<Object, Void, Object> {
    private final WeakReference<OfflineManager> offlineManagerWeakReference;
    private final WeakReference<MergeOfflineRegionsCallback> callbackWeakReference;

    CopyTempDatabaseFileTask(OfflineManager offlineManager, MergeOfflineRegionsCallback callback) {
      this.offlineManagerWeakReference = new WeakReference<>(offlineManager);
      this.callbackWeakReference = new WeakReference<>(callback);
    }

    @Override
    protected Object doInBackground(Object... objects) {
      File src = (File) objects[0];
      File dst = (File) objects[1];

      try {
        copyTempDatabaseFile(src, dst);
        return dst;
      } catch (IOException ex) {
        return ex.getMessage();
      }
    }

    @Override
    protected void onPostExecute(Object object) {
      MergeOfflineRegionsCallback callback = callbackWeakReference.get();
      if (callback != null) {
        OfflineManager offlineManager = offlineManagerWeakReference.get();
        if (object instanceof File && offlineManager != null) {
          // successfully copied the file, perform merge
          File dst = (File) object;
          offlineManager.mergeOfflineDatabaseFiles(dst, callback, true);
        } else if (object instanceof String) {
          // error occurred
          callback.onError((String) object);
        }
      }
    }
  }

  private static void copyTempDatabaseFile(File sourceFile, File destFile) throws IOException {
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

  private void mergeOfflineDatabaseFiles(@NonNull File file, @NonNull final MergeOfflineRegionsCallback callback,
                                         boolean isTemporaryFile) {
    fileSource.activate();
    mergeOfflineRegions(fileSource, file.getAbsolutePath(), new MergeOfflineRegionsCallback() {
      @Override
      public void onMerge(OfflineRegion[] offlineRegions) {
        getHandler().post(new Runnable() {
          @Override
          public void run() {
            fileSource.deactivate();
            if (isTemporaryFile) {
              file.delete();
            }
            callback.onMerge(offlineRegions);
          }
        });
      }

      @Override
      public void onError(String error) {
        getHandler().post(new Runnable() {
          @Override
          public void run() {
            fileSource.deactivate();
            if (isTemporaryFile) {
              file.delete();
            }
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
                                  final CreateOfflineRegionCallback callback) {
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
        getHandler().post(new Runnable() {
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
        getHandler().post(new Runnable() {
          @Override
          public void run() {
            ConnectivityReceiver.instance(context).deactivate();
            FileSource.getInstance(context).deactivate();
            callback.onError(error);
          }
        });
      }
    });
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
}
