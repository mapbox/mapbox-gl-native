package com.mapbox.mapboxsdk.offline;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.NonNull;

import java.io.File;

/**
 * The offline manager is the main entry point for offline-related functionality.
 * It'll help you list and create offline regions.
 */
public class OfflineManager {

    // Default database name
    private final static String OFFLINE_DATABASE_NAME = "mbgl-offline.db";

    /*
     * The maximumCacheSize parameter is a limit applied to non-offline resources only,
     * i.e. resources added to the database for the "ambient use" caching functionality.
     * There is no size limit for offline resources.
     */
    private final static long DEFAULT_MAX_CACHE_SIZE = 50 * 1024 * 1024;

    // Holds the pointer to JNI DefaultFileSource
    private long mDefaultFileSourcePtr = 0;

    // Makes sure callbacks come back to the main thread
    private Handler handler;

    // This object is implemented as a singleton
    private static OfflineManager instance;

    /*
     * Callbacks
     */

    public interface ListOfflineRegionsCallback {
        void onList(OfflineRegion[] offlineRegions);
        void onError(String error);
    }

    public interface CreateOfflineRegionCallback {
        void onCreate(OfflineRegion offlineRegion);
        void onError(String error);
    }

    /*
     * Constructors
     */

    private OfflineManager(Context context) {
        // Get a pointer to the DefaultFileSource instance
        String assetRoot = context.getFilesDir().getAbsolutePath();
        String cachePath = assetRoot  + File.separator + OFFLINE_DATABASE_NAME;
        mDefaultFileSourcePtr = createDefaultFileSource(cachePath, assetRoot, DEFAULT_MAX_CACHE_SIZE);
    }

    public static synchronized OfflineManager getInstance(Context context) {
        if (instance == null) {
            instance = new OfflineManager(context);
        }

        return instance;
    }

    /*
     * Access token getter/setter
     */
    public void setAccessToken(String accessToken) {
        setAccessToken(mDefaultFileSourcePtr, accessToken);
    }

    public String getAccessToken() {
        return getAccessToken(mDefaultFileSourcePtr);
    }

    private Handler getHandler() {
        if (handler == null) {
            handler = new Handler(Looper.getMainLooper());
        }

        return handler;
    }

    /**
     * Retrieve all regions in the offline database.
     *
     * The query will be executed asynchronously and the results passed to the given
     * callback on the main thread.
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
     *
     * When the initial database queries have completed, the provided callback will be
     * executed on the main thread.
     *
     * Note that the resulting region will be in an inactive download state; to begin
     * downloading resources, call `OfflineRegion.setDownloadState(DownloadState.STATE_ACTIVE)`,
     * optionally registering an `OfflineRegionObserver` beforehand.
     */
    public void createOfflineRegion(
            @NonNull OfflineRegionDefinition definition,
            @NonNull OfflineRegionMetadata metadata,
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
            OfflineRegionMetadata metadata, CreateOfflineRegionCallback callback);

    private native void setOfflineMapboxTileCountLimit(
            long defaultFileSourcePtr, long limit);

}
