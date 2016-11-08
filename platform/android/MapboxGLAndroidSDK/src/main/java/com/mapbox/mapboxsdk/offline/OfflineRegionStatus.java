package com.mapbox.mapboxsdk.offline;

/**
 * A region's status includes its active/inactive state as well as counts
 * of the number of resources that have completed downloading, their total
 * size in bytes, and the total number of resources that are required.
 * <p>
 * Note that the total required size in bytes is not currently available. A
 * future API release may provide an estimate of this number.
 * </p>
 */
public class OfflineRegionStatus {

    @OfflineRegion.DownloadState
    private int downloadState = OfflineRegion.STATE_INACTIVE;
    private long completedResourceCount = 0;
    private long completedResourceSize = 0;
    private long completedTileCount = 0;
    private long completedTileSize = 0;
    private long requiredResourceCount = 0;
    private boolean requiredResourceCountIsPrecise = true;

    /*
     * Use setObserver(OfflineRegionObserver observer) to obtain a OfflineRegionStatus object.
     */

    private OfflineRegionStatus() {
        // For JNI use only
    }

    /*
     * Is the region complete?
     */

    public boolean isComplete() {
        return (completedResourceCount == requiredResourceCount);
    }

    /*
     * Getters
     */

    public @OfflineRegion.DownloadState int getDownloadState() {
        return downloadState;
    }

    /**
     * The number of resources (inclusive of tiles) that have been fully downloaded
     * and are ready for offline access.
     *
     * @return a long value representing the total downloaded resource count.
     */
    public long getCompletedResourceCount() {
        return completedResourceCount;
    }

    /**
     * The cumulative size, in bytes, of all resources (inclusive of tiles) that have
     * been fully downloaded.
     *
     * @return a long value representing the total downloaded resource size in bytes.
     */
    public long getCompletedResourceSize() {
        return completedResourceSize;
    }

    /**
     * The number of tiles that have been fully downloaded and are ready for
     * offline access.
     *
     * @return a long value representing the total downloaded tile count.
     */
    public long getCompletedTileCount() {
        return completedTileCount;
    }

    /**
     * The cumulative size, in bytes, of all tiles that have been fully downloaded.
     *
     * @return a long value representing the total downloaded tile count size in bytes.
     */
    public long getCompletedTileSize() {
        return completedTileSize;
    }

    /**
     * The number of resources that are known to be required for this region. See the
     * documentation for `requiredResourceCountIsPrecise` for an important caveat
     * about this number.
     *
     * @return a long value representing the total known tile count size required for given
     * the region.
     */
    public long getRequiredResourceCount() {
        return requiredResourceCount;
    }

    /**
     * This property is true when the value of requiredResourceCount is a precise
     * count of the number of required resources, and false when it is merely a lower
     * bound.
     * <p>
     * Specifically, it is false during early phases of an offline download. Once
     * style and tile sources have been downloaded, it is possible to calculate the
     * precise number of required resources, at which point it is set to true.
     *
     * @return true when {@link OfflineRegionStatus#getRequiredResourceCount()} is a precise count,
     * else false.
     */
    public boolean isRequiredResourceCountPrecise() {
        return requiredResourceCountIsPrecise;
    }

}
