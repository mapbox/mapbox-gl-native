package com.mapbox.mapboxsdk.offline;

import com.mapbox.mapboxsdk.geometry.LatLngBounds;

/**
 * An offline region defined by a style URL, geographic bounding box, zoom range, and
 * device pixel ratio.
 * <p>
 * Both minZoom and maxZoom must be ≥ 0, and maxZoom must be ≥ minZoom.
 * <p>
 * maxZoom may be ∞, in which case for each tile source, the region will include
 * tiles from minZoom up to the maximum zoom level provided by that source.
 * <p>
 * pixelRatio must be ≥ 0 and should typically be 1.0 or 2.0.
 */
public class OfflineTilePyramidRegionDefinition implements OfflineRegionDefinition {

  private String styleURL;
  private LatLngBounds bounds;
  private double minZoom;
  private double maxZoom;
  private float pixelRatio;

  /*
   * Constructors
   */

  private OfflineTilePyramidRegionDefinition() {
    // For JNI use only
  }

  public OfflineTilePyramidRegionDefinition(
    String styleURL, LatLngBounds bounds, double minZoom, double maxZoom, float pixelRatio) {
    this.styleURL = styleURL;
    this.bounds = bounds;
    this.minZoom = minZoom;
    this.maxZoom = maxZoom;
    this.pixelRatio = pixelRatio;
  }

  /*
   * Getters
   */

  public String getStyleURL() {
    return styleURL;
  }

  public LatLngBounds getBounds() {
    return bounds;
  }

  public double getMinZoom() {
    return minZoom;
  }

  public double getMaxZoom() {
    return maxZoom;
  }

  public float getPixelRatio() {
    return pixelRatio;
  }

}
