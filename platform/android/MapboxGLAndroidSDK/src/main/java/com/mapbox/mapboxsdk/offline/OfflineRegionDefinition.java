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

  LatLngBounds getBounds();

}
