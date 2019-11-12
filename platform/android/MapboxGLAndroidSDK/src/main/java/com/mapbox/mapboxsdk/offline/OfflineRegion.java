package com.mapbox.mapboxsdk.offline;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.IntDef;
import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import com.mapbox.mapboxsdk.LibraryLoader;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.net.ConnectivityReceiver;
import com.mapbox.mapboxsdk.storage.FileSource;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * An offline region is the basic building block for offline mobile maps.
 * Use {@link com.mapbox.mapboxsdk.offline.OfflineManager.CreateOfflineRegionCallback}
 * to create a new offline region.
 */
public class OfflineRegion {

  //
  // Static methods
  //

  static {
    LibraryLoader.load();
  }

  // Members

  // Application context
  private final Context context;

  // Holds the pointer to JNI OfflineRegion
  @Keep
  private long nativePtr;

  // Holds a reference to the FileSource to keep it alive
  private FileSource fileSource;

  //Region id
  private long id;

  // delete status
  private boolean isDeleted;

  private OfflineRegionDefinition definition;

  /**
   * Arbitrary binary region metadata. The contents are opaque to the SDK implementation;
   * it just stores and retrieves a byte[]. Check the `OfflineActivity` in the TestApp
   * for a sample implementation that uses JSON to store an offline region name.
   */
  private byte[] metadata;

  // Makes sure callbacks come back to the main thread
  private final Handler handler = new Handler(Looper.getMainLooper());

  /**
   * A region can have a single observer, which gets notified whenever a change
   * to the region's status occurs.
   */
  @Keep
  public interface OfflineRegionObserver {
    /**
     * Implement this method to be notified of a change in the status of an
     * offline region. Status changes include any change in state of the members
     * of OfflineRegionStatus.
     * <p>
     * This method will be executed on the main thread.
     * </p>
     *
     * @param status the changed status
     */
    void onStatusChanged(OfflineRegionStatus status);

    /**
     * Implement this method to be notified of errors encountered while downloading
     * regional resources. Such errors may be recoverable; for example the implementation
     * will attempt to re-request failed resources based on an exponential backoff
     * algorithm, or when it detects that network access has been restored.
     * <p>
     * This method will be executed on the main thread.
     * </p>
     *
     * @param error the offline region error message
     */
    void onError(OfflineRegionError error);

    /*
     * Implement this method to be notified when the limit on the number of Mapbox
     * tiles stored for offline regions has been reached.
     *
     * Once the limit has been reached, the SDK will not download further offline
     * tiles from Mapbox APIs until existing tiles have been removed.
     *
     * This limit does not apply to non-Mapbox tile sources.
     *
     * This method will be executed on the main thread.
     */
    void mapboxTileCountLimitExceeded(long limit);
  }

  /**
   * This callback receives an asynchronous response containing the OfflineRegionStatus
   * of the offline region, or a {@link String} error message otherwise.
   */
  @Keep
  public interface OfflineRegionStatusCallback {
    /**
     * Receives the status
     *
     * @param status the offline region status
     */
    void onStatus(OfflineRegionStatus status);

    /**
     * Receives the error message
     *
     * @param error the error message
     */
    void onError(String error);
  }

  /**
   * This callback receives an asynchronous response containing a notification when
   * an offline region has been deleted, or a {@link String} error message otherwise.
   */
  @Keep
  public interface OfflineRegionDeleteCallback {
    /**
     * Receives the delete notification
     */
    void onDelete();

    /**
     * Receives the error message
     *
     * @param error the error message
     */
    void onError(String error);
  }

  /**
   * This callback receives an asynchronous response containing a notification when
   * an offline region has been invalidated, or a {@link String} error message otherwise.
   */
  @Keep
  public interface OfflineRegionInvalidateCallback {
    /**
     * Receives the invalidate notification
     */
    void onInvalidate();

    /**
     * Receives the error message
     *
     * @param error the error message
     */
    void onError(String error);
  }

  /**
   * This callback receives an asynchronous response containing the newly update
   * OfflineMetadata in the database, or an error message otherwise.
   */
  @Keep
  public interface OfflineRegionUpdateMetadataCallback {
    /**
     * Receives the newly update offline region metadata.
     *
     * @param metadata the offline metadata to u[date
     */
    void onUpdate(byte[] metadata);

    /**
     * Receives the error message.
     *
     * @param error the error message to be shown
     */
    void onError(String error);
  }

  /**
   * A region is either inactive (not downloading, but previously-downloaded
   * resources are available for use), or active (resources are being downloaded
   * or will be downloaded, if necessary, when network access is available).
   * <p>
   * This state is independent of whether or not the complete set of resources
   * is currently available for offline use. To check if that is the case, use
   * `OfflineRegionStatus.isComplete()`.
   * </p>
   */

  @IntDef( {STATE_INACTIVE, STATE_ACTIVE})
  @Retention(RetentionPolicy.SOURCE)
  public @interface DownloadState {
  }

  public static final int STATE_INACTIVE = 0;
  public static final int STATE_ACTIVE = 1;

  // Keep track of the region state
  private int state = STATE_INACTIVE;

  private boolean deliverInactiveMessages = false;

  /**
   * Gets whether or not the `OfflineRegionObserver` will continue to deliver messages even if
   * the region state has been set as STATE_INACTIVE.
   *
   * @return true if delivering inactive messages
   */
  public boolean isDeliveringInactiveMessages() {
    return deliverInactiveMessages;
  }

  /**
   * When set true, the `OfflineRegionObserver` will continue to deliver messages even if
   * the region state has been set as STATE_INACTIVE (operations happen asynchronously). If set
   * false, the client won't be notified of further messages.
   *
   * @param deliverInactiveMessages true if it should deliver inactive messages
   */
  public void setDeliverInactiveMessages(boolean deliverInactiveMessages) {
    this.deliverInactiveMessages = deliverInactiveMessages;
  }

  private boolean deliverMessages() {
    if (state == STATE_ACTIVE) {
      return true;
    }
    return isDeliveringInactiveMessages();
  }

  /**
   * Constructor
   * <p>
   * For JNI use only, to create a new offline region, use
   * {@link OfflineManager#createOfflineRegion} instead.
   */
  @Keep
  private OfflineRegion(long offlineRegionPtr, FileSource fileSource, long id,
                        OfflineRegionDefinition definition, byte[] metadata) {
    this.context = Mapbox.getApplicationContext();
    this.fileSource = fileSource;
    this.id = id;
    this.definition = definition;
    this.metadata = metadata;
    initialize(offlineRegionPtr, fileSource);
  }

  /*
   * Getters
   */

  public long getID() {
    return id;
  }

  public OfflineRegionDefinition getDefinition() {
    return definition;
  }

  public byte[] getMetadata() {
    return metadata;
  }

  /**
   * Register an observer to be notified when the state of the region changes.
   *
   * @param observer the observer to be notified
   */
  public void setObserver(@Nullable final OfflineRegionObserver observer) {
    setOfflineRegionObserver(new OfflineRegionObserver() {
      @Override
      public void onStatusChanged(final OfflineRegionStatus status) {
        if (deliverMessages()) {
          handler.post(new Runnable() {
            @Override
            public void run() {
              if (observer != null) {
                observer.onStatusChanged(status);
              }
            }
          });
        }
      }

      @Override
      public void onError(final OfflineRegionError error) {
        if (deliverMessages()) {
          handler.post(new Runnable() {
            @Override
            public void run() {
              if (observer != null) {
                observer.onError(error);
              }
            }
          });
        }
      }

      @Override
      public void mapboxTileCountLimitExceeded(final long limit) {
        if (deliverMessages()) {
          handler.post(new Runnable() {
            @Override
            public void run() {
              if (observer != null) {
                observer.mapboxTileCountLimitExceeded(limit);
              }
            }
          });
        }
      }
    });
  }

  /**
   * Pause or resume downloading of regional resources.
   * <p>
   * After a download has been completed, you are required to reset the state of the region to STATE_INACTIVE.
   * </p>
   *
   * @param state the download state
   */
  public void setDownloadState(@DownloadState int state) {
    if (this.state == state) {
      return;
    }

    if (state == STATE_ACTIVE) {
      ConnectivityReceiver.instance(context).activate();
      fileSource.activate();
    } else {
      fileSource.deactivate();
      ConnectivityReceiver.instance(context).deactivate();
    }

    this.state = state;
    setOfflineRegionDownloadState(state);
  }

  /**
   * Retrieve the current status of the region. The query will be executed
   * asynchronously and the results passed to the given callback which will be
   * executed on the main thread.
   *
   * @param callback the callback to invoked.
   */
  public void getStatus(@NonNull final OfflineRegionStatusCallback callback) {
    fileSource.activate();
    getOfflineRegionStatus(new OfflineRegionStatusCallback() {
      @Override
      public void onStatus(final OfflineRegionStatus status) {
        handler.post(new Runnable() {
          @Override
          public void run() {
            fileSource.deactivate();
            callback.onStatus(status);
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
   * Remove an offline region from the database and perform any resources evictions
   * necessary as a result.
   * <p>
   * Eviction works by removing the least-recently requested resources not also required
   * by other regions, until the database shrinks below a certain size.
   * </p>
   * <p>
   * When the operation is complete or encounters an error, the given callback will be
   * executed on the main thread.
   * </p>
   * <p>
   * After you call this method, you may not call any additional methods on this object.
   * </p>
   *
   * @param callback the callback to be invoked
   */
  public void delete(@NonNull final OfflineRegionDeleteCallback callback) {
    if (!isDeleted) {
      isDeleted = true;
      fileSource.activate();
      deleteOfflineRegion(true /*pack*/, new OfflineRegionDeleteCallback() {
        @Override
        public void onDelete() {
          handler.post(new Runnable() {
            @Override
            public void run() {
              fileSource.deactivate();
              callback.onDelete();
              OfflineRegion.this.finalize();
            }
          });
        }

        @Override
        public void onError(final String error) {
          handler.post(new Runnable() {
            @Override
            public void run() {
              isDeleted = false;
              fileSource.deactivate();
              callback.onError(error);
            }
          });
        }
      });
    }
  }

  /**
   * Same as {@link OfflineRegion#delete} but skipping database file packing for performance reasons.
   * <p>
   * Database file packing can be done later with {@link OfflineManager#packDatabase}.
   * This method is a useful optimization e.g. when several regions should be deleted in a row.
   * </p>
   * <p>
   * When the operation is complete or encounters an error, the given callback will be
   * executed on the main thread.
   * </p>
   * <p>
   * After you call this method, you may not call any additional methods on this object.
   * </p>
   *
   * @param callback the callback to be invoked
   */
  public void deleteAndSkipPackDatabase(@NonNull final OfflineRegionDeleteCallback callback) {
    if (!isDeleted) {
      isDeleted = true;
      fileSource.activate();
      deleteOfflineRegion(false /*pack*/, new OfflineRegionDeleteCallback() {
        @Override
        public void onDelete() {
          handler.post(new Runnable() {
            @Override
            public void run() {
              fileSource.deactivate();
              callback.onDelete();
              OfflineRegion.this.finalize();
            }
          });
        }

        @Override
        public void onError(final String error) {
          handler.post(new Runnable() {
            @Override
            public void run() {
              isDeleted = false;
              fileSource.deactivate();
              callback.onError(error);
            }
          });
        }
      });
    }
  }

  /**
   * Invalidate all the tiles from an offline region forcing Mapbox GL to revalidate
   * the tiles with the server before using. This is more efficient than deleting the
   * offline region and downloading it again because if the data on the cache matches
   * the server, no new data gets transmitted.
   *
   * @param callback the callback to be invoked
   */
  public void invalidate(@Nullable final OfflineRegionInvalidateCallback callback) {
    fileSource.activate();
    invalidateOfflineRegion(new OfflineRegionInvalidateCallback() {

      @Override
      public void onInvalidate() {
        handler.post(new Runnable() {
          @Override
          public void run() {
            fileSource.deactivate();
            if (callback != null) {
              callback.onInvalidate();
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
   * Update an offline region metadata from the database.
   * <p>
   * When the operation is complete or encounters an error, the given callback will be
   * executed on the main thread.
   * </p>
   *
   * @param bytes    the metadata in bytes
   * @param callback the callback to be invoked
   */
  public void updateMetadata(@NonNull final byte[] bytes, @NonNull final OfflineRegionUpdateMetadataCallback callback) {
    updateOfflineRegionMetadata(bytes, new OfflineRegionUpdateMetadataCallback() {
      @Override
      public void onUpdate(final byte[] metadata) {
        handler.post(new Runnable() {
          @Override
          public void run() {
            OfflineRegion.this.metadata = metadata;
            callback.onUpdate(metadata);
          }
        });
      }

      @Override
      public void onError(final String error) {
        handler.post(new Runnable() {
          @Override
          public void run() {
            callback.onError(error);
          }
        });
      }
    });
  }

  @Keep
  private native void initialize(long offlineRegionPtr, FileSource fileSource);

  @Override
  @Keep
  protected native void finalize();

  @Keep
  private native void setOfflineRegionObserver(OfflineRegionObserver callback);

  @Keep
  private native void setOfflineRegionDownloadState(@DownloadState int offlineRegionDownloadState);

  @Keep
  private native void getOfflineRegionStatus(OfflineRegionStatusCallback callback);

  @Keep
  private native void deleteOfflineRegion(boolean pack, OfflineRegionDeleteCallback callback);

  @Keep
  private native void updateOfflineRegionMetadata(byte[] metadata, OfflineRegionUpdateMetadataCallback callback);

  @Keep
  private native void invalidateOfflineRegion(OfflineRegionInvalidateCallback callback);

}
