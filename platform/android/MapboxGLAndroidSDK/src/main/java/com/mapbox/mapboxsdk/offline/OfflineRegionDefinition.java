package com.mapbox.mapboxsdk.offline;

import com.mapbox.mapboxsdk.geometry.LatLngBounds;

/**
 * This is the interface that all Offline Region definitions have to implement.
 * <p>
 * For the present, a tile pyramid is the only type of offline region.
 */
public interface OfflineRegionDefinition {

  LatLngBounds getBounds();

}
