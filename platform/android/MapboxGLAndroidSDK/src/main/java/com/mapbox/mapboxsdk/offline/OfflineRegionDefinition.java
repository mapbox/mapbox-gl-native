package com.mapbox.mapboxsdk.offline;

import android.os.Parcelable;
import androidx.annotation.Keep;

import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;

/**
 * This is the interface that all Offline Region definitions have to implement.
 * <p>
 * For the present, a tile pyramid is the only type of offline region.
 */
@Keep
public interface OfflineRegionDefinition extends Parcelable {

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
   * Specifies whether to include ideographic glyphs in downloaded font data.
   * Ideographic glyphs make up the majority of downloaded font data, but
   * it is possible to configure the renderer to use locally installed fonts
   * instead of relying on fonts downloaded as part of the offline pack.
   *
   * Defaults to `true`
   *
   * @return true if offline region will include ideographic glyphs
   * @see MapboxMapOptions#localIdeographFontFamily(String)
   */
  boolean getIncludeIdeographs();

  /**
   * Gets the type of the OfflineRegionDefinition for telemetry ("tileregion", "shaperegion").
   *
   * @return The type of the OfflineRegionDefinition.
   */
  String getType();

}
