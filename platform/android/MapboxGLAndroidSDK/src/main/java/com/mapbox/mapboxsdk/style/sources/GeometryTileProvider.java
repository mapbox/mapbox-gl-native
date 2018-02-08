package com.mapbox.mapboxsdk.style.sources;

import android.support.annotation.WorkerThread;

import com.mapbox.geojson.FeatureCollection;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;

/**
 * Interface that defines methods for working with {@link CustomGeometrySource}.
 */
public interface GeometryTileProvider {

  /***
   * Interface method called by {@link CustomGeometrySource} to request features for a tile.
   *
   * @param bounds {@link LatLngBounds} of the tile.
   * @param zoomLevel Tile zoom level.
   * @return Return a @{link FeatureCollection} to be displayed in the requested tile.
   */
  @WorkerThread
  FeatureCollection getFeaturesForBounds(LatLngBounds bounds, int zoomLevel);
}
