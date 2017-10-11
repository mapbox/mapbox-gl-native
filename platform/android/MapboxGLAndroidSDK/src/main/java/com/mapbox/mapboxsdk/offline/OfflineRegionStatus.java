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
   * </p>
   */
  private boolean requiredResourceCountIsPrecise = true;

  /*
   * Use setObserver(OfflineRegionObserver observer) to obtain a OfflineRegionStatus object.
   *
   * For JNI use only
   */
  private OfflineRegionStatus(int downloadState, long completedResourceCount,
                              long completedResourceSize, long completedTileCount,
                              long completedTileSize, long requiredResourceCount,
                              boolean requiredResourceCountIsPrecise) {
    this.downloadState = downloadState;
    this.completedResourceCount = completedResourceCount;
    this.completedResourceSize = completedResourceSize;
    this.completedTileCount = completedTileCount;
    this.completedTileSize = completedTileSize;
    this.requiredResourceCount = requiredResourceCount;
    this.requiredResourceCountIsPrecise = requiredResourceCountIsPrecise;
  }

  /**
   * Validates if the region download has completed
   *
   * @return true if download is complete, false if not
   */
  public boolean isComplete() {
    return (completedResourceCount == requiredResourceCount) && downloadState == OfflineRegion.STATE_INACTIVE;
  }

  /**
   * Returns the download state.
   * <p>
   * State is defined as
   * </p>
   * <ul>
   * <li>{@link OfflineRegion#STATE_ACTIVE}</li>
   * <li>{@link OfflineRegion#STATE_INACTIVE}</li>
   * </ul>
   *
   * @return the download state.
   */
  @OfflineRegion.DownloadState
  public int getDownloadState() {
    return downloadState;
  }

  /**
   * Get the number of resources (inclusive of tiles) that have been fully downloaded
   * and are ready for offline access.
   *
   * @return the amount of resources that have finished downloading.
   */
  public long getCompletedResourceCount() {
    return completedResourceCount;
  }

  /**
   * The cumulative size, in bytes, of all resources (inclusive of tiles) that have
   * been fully downloaded.
   *
   * @return the size of the resources that have finished downloading
   */
  public long getCompletedResourceSize() {
    return completedResourceSize;
  }

  /**
   * Get the number of tiles that have been fully downloaded and are ready for
   * offline access.
   *
   * @return the completed tile count
   */
  public long getCompletedTileCount() {
    return completedTileCount;
  }

  /**
   * Get the cumulative size, in bytes, of all tiles that have been fully downloaded.
   *
   * @return the completed tile size
   */
  public long getCompletedTileSize() {
    return completedTileSize;
  }

  /**
   * Get the number of resources that are known to be required for this region. See the
   * documentation for `requiredResourceCountIsPrecise` for an important caveat
   * about this number.
   *
   * @return the amount of resources that are required
   */
  public long getRequiredResourceCount() {
    return requiredResourceCount;
  }

  /**
   * Returns when the value of requiredResourceCount is a precise
   * count of the number of required resources, and false when it is merely a lower
   * bound.
   * <p>
   * Specifically, it is false during early phases of an offline download. Once
   * style and tile sources have been downloaded, it is possible to calculate the
   * precise number of required resources, at which point it is set to true.
   * </p>
   *
   * @return True if the required resource count is precise, false if not
   */
  public boolean isRequiredResourceCountPrecise() {
    return requiredResourceCountIsPrecise;
  }

}
