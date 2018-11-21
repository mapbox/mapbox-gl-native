package com.mapbox.mapboxsdk.offline;

import android.support.annotation.Keep;

import com.mapbox.mapboxsdk.geometry.LatLngBounds;

/**
 * This is the interface that all Offline Region definitions have to implement.
 * <p>
 * For the present, a tile pyramid is the only type of offline region.
 */
@Keep
public interface OfflineRegionDefinition {

  /**
   * Gets the bounds of the OfflineRegion.
   *
   * @return the bounds of the OfflineRegion.
   */
  LatLngBounds getBounds();

  /**
   * Returns the map style url of the OfflineRegion.
   *
   * @return The URL of the map style.
   */
  String getStyleURL();

  /**
   * Gets the minimum zoom level the OfflineRegion map can be displayed at.
   *
   * @return The minimum zoom level.
   */
  double getMinZoom();

  /**
   * Gets the maximum zoom level the OfflineRegion map can be displayed at.
   *
   * @return The maximum zoom level.
   */
  double getMaxZoom();

  /**
   * Gets the pixel ratio of the OfflineRegion map.
   *
   * @return The pixel ratio of the OfflineRegion map.
   */
  float getPixelRatio();

  /**
   * Gest the type of the OfflineRegionDefinition for telemetry ("tileregion", "shaperegion").
   *
   * @return The type of the OfflineRegionDefinition.
   */
  String getType();

}
