package com.mapbox.mapboxsdk.offline;

/**
 * A region's status includes its active/inactive state as well as counts
 * of the number of resources that have completed downloading, their total
 * size in bytes, and the total number of resources that are required.
 * <p>
 * Note that the total required size in bytes is not currently available. A
 * future API release may provide an estimate of this number.
 */
public class OfflineRegionStatus {

  @OfflineRegion.DownloadState
  private int downloadState = OfflineRegion.STATE_INACTIVE;

  /**
   * The number of resources (inclusive of tiles) that have been fully downloaded
   * and are ready for offline access.
   */
  private long completedResourceCount = 0;

  /**
   * The cumulative size, in bytes, of all resources (inclusive of tiles) that have
   * been fully downloaded.
   */
  private long completedResourceSize = 0;

  /**
   * The number of tiles that have been fully downloaded and are ready for
   * offline access.
   */
  private long completedTileCount = 0;

  /**
   * The cumulative size, in bytes, of all tiles that have been fully downloaded.
   */
  private long completedTileSize = 0;

  /**
   * The number of resources that are known to be required for this region. See the
   * documentation for `requiredResourceCountIsPrecise` for an important caveat
   * about this number.
   */
  private long requiredResourceCount = 0;

  /**
   * This property is true when the value of requiredResourceCount is a precise
   * count of the number of required resources, and false when it is merely a lower
   * bound.
   * <p>
   * Specifically, it is false during early phases of an offline download. Once
   * style and tile sources have been downloaded, it is possible to calculate the
   * precise number of required resources, at which point it is set to true.
   */
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

  @OfflineRegion.DownloadState
  public int getDownloadState() {
    return downloadState;
  }

  public long getCompletedResourceCount() {
    return completedResourceCount;
  }

  public long getCompletedResourceSize() {
    return completedResourceSize;
  }

  public long getCompletedTileCount() {
    return completedTileCount;
  }

  public long getCompletedTileSize() {
    return completedTileSize;
  }

  public long getRequiredResourceCount() {
    return requiredResourceCount;
  }

  public boolean isRequiredResourceCountPrecise() {
    return requiredResourceCountIsPrecise;
  }

}
