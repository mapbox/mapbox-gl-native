package com.mapbox.mapboxsdk.offline;

import android.os.Handler;
import android.os.Looper;
import android.support.annotation.IntDef;
import android.support.annotation.NonNull;
import android.util.Log;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * An offline region is the basic building block for offline mobile maps.
 * Use {@link com.mapbox.mapboxsdk.offline.OfflineManager.CreateOfflineRegionCallback}
 * to create a new offline region.
 */
public class OfflineRegion {

    private final static String LOG_TAG = "OfflineRegion";

    //
    // Static methods
    //

    static {
        System.loadLibrary("mapbox-gl");
    }

    // Parent OfflineManager
    private OfflineManager offlineManager;

    // Members
    private long mId = 0;
    private OfflineRegionDefinition mDefinition = null;

    /**
     * Arbitrary binary region metadata. The contents are opaque to the SDK implementation;
     * it just stores and retrieves a byte[]. Check the `OfflineActivity` in the TestApp
     * for a sample implementation that uses JSON to store an offline region name.
     */
    private byte[] mMetadata = null;

    // Holds the pointer to JNI OfflineRegion
    private long mOfflineRegionPtr = 0;

    // Makes sure callbacks come back to the main thread
    private Handler handler;

    /**
     * A region can have a single observer, which gets notified whenever a change
     * to the region's status occurs.
     */
    public interface OfflineRegionObserver {
        /**
         * Implement this method to be notified of a change in the status of an
         * offline region. Status changes include any change in state of the members
         * of OfflineRegionStatus.
         *
         * This method will be executed on the main thread.
         */
        void onStatusChanged(OfflineRegionStatus status);

        /**
         * Implement this method to be notified of errors encountered while downloading
         * regional resources. Such errors may be recoverable; for example the implementation
         * will attempt to re-request failed resources based on an exponential backoff
         * algorithm, or when it detects that network access has been restored.
         *
         * This method will be executed on the main thread.
         */
        void onError(OfflineRegionError error);

        /*
         * Implement this method to be notified when the limit on the number of Mapbox
         * tiles stored for offline regions has been reached.
         *
         * Once the limit has been reached, the SDK will not download further offline
         * tiles from Mapbox APIs until existing tiles have been removed. Contact your
         * Mapbox sales representative to raise the limit.
         *
         * This limit does not apply to non-Mapbox tile sources.
         *
         * This method will be executed on the main thread.
         */
        void mapboxTileCountLimitExceeded(long limit);
    }

    /**
     * This callback receives an asynchronous response containing the {@link OfflineRegionStatus}
     * of the offline region, or a {@link String} error message otherwise.
     */
    public interface OfflineRegionStatusCallback {
        /**
         * Receives the status
         *
         * @param status
         */
        void onStatus(OfflineRegionStatus status);

        /**
         * Receives the error message
         *
         * @param error
         */
        void onError(String error);
    }

    /**
     * This callback receives an asynchronous response containing a notification when
     * an offline region has been deleted, or a {@link String} error message otherwise.
     */
    public interface OfflineRegionDeleteCallback {
        /**
         * Receives the delete notification
         */
        void onDelete();

        /**
         * Receives the error message
         *
         * @param error
         */
        void onError(String error);
    }

    /**
     * A region is either inactive (not downloading, but previously-downloaded
     * resources are available for use), or active (resources are being downloaded
     * or will be downloaded, if necessary, when network access is available).
     *
     * This state is independent of whether or not the complete set of resources
     * is currently available for offline use. To check if that is the case, use
     * `OfflineRegionStatus.isComplete()`.
     */

    @IntDef({STATE_INACTIVE, STATE_ACTIVE})
    @Retention(RetentionPolicy.SOURCE)
    public @interface DownloadState {}

    public static final int STATE_INACTIVE = 0;
    public static final int STATE_ACTIVE = 1;

    /*
     * Constructor
     */

    private OfflineRegion() {
        // For JNI use only, to create a new offline region, use
        // OfflineManager.createOfflineRegion() instead.
    }

    /*
     * Getters
     */

    public long getID() {
        return mId;
    }

    public OfflineRegionDefinition getDefinition() {
        return mDefinition;
    }

    public byte[] getMetadata() {
        return mMetadata;
    }

    private Handler getHandler() {
        if (handler == null) {
            handler = new Handler(Looper.getMainLooper());
        }

        return handler;
    }

    /**
     * Register an observer to be notified when the state of the region changes.
     */
    public void setObserver(@NonNull final OfflineRegionObserver observer) {
        setOfflineRegionObserver(new OfflineRegionObserver() {
            @Override
            public void onStatusChanged(final OfflineRegionStatus status) {
                getHandler().post(new Runnable() {
                    @Override
                    public void run() {
                        observer.onStatusChanged(status);
                    }
                });
            }

            @Override
            public void onError(final OfflineRegionError error) {
                getHandler().post(new Runnable() {
                    @Override
                    public void run() {
                        observer.onError(error);
                    }
                });
            }

            @Override
            public void mapboxTileCountLimitExceeded(final long limit) {
                getHandler().post(new Runnable() {
                    @Override
                    public void run() {
                        observer.mapboxTileCountLimitExceeded(limit);
                    }
                });
            }
        });
    }

    /**
     * Pause or resume downloading of regional resources.
     */
    public void setDownloadState(@DownloadState int state) {
        setOfflineRegionDownloadState(state);
    }

    /**
     * Retrieve the current status of the region. The query will be executed
     * asynchronously and the results passed to the given callback which will be
     * executed on the main thread.
     */
    public void getStatus(@NonNull final OfflineRegionStatusCallback callback) {
        getOfflineRegionStatus(new OfflineRegionStatusCallback() {
            @Override
            public void onStatus(final OfflineRegionStatus status) {
                getHandler().post(new Runnable() {
                    @Override
                    public void run() {
                        callback.onStatus(status);
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
     * Remove an offline region from the database and perform any resources evictions
     * necessary as a result.
     *
     * Eviction works by removing the least-recently requested resources not also required
     * by other regions, until the database shrinks below a certain size.
     *
     * When the operation is complete or encounters an error, the given callback will be
     * executed on the main thread.
     *
     * After you call this method, you may not call any additional methods on this object.
     */
    public void delete(@NonNull final OfflineRegionDeleteCallback callback) {
        deleteOfflineRegion(new OfflineRegionDeleteCallback() {
            @Override
            public void onDelete() {
                getHandler().post(new Runnable() {
                    @Override
                    public void run() {
                        callback.onDelete();
                        OfflineRegion.this.finalize();
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

    @Override
    protected void finalize() {
        try {
            super.finalize();
            destroyOfflineRegion();
        } catch (Throwable throwable) {
            Log.e(LOG_TAG, "Failed to finalize OfflineRegion: " + throwable.getMessage());
        }
    }

    /*
     * Native methods
     */

    private native void destroyOfflineRegion();

    private native void setOfflineRegionObserver(
            OfflineRegionObserver observerCallback);

    private native void setOfflineRegionDownloadState(
            @DownloadState int offlineRegionDownloadState);

    private native void getOfflineRegionStatus(
            OfflineRegionStatusCallback statusCallback);

    private native void deleteOfflineRegion(
            OfflineRegionDeleteCallback deleteCallback);

}
