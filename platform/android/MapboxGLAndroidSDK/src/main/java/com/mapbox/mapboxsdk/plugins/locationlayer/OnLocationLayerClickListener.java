package com.mapbox.mapboxsdk.plugins.locationlayer;

/**
 * The Location Layer Plugin exposes an API for listening to when the user clicks on the location
 * layer icon visible on the map. when this event occurs, the {@link #onLocationLayerClick()} method
 * gets invoked.
 */
public interface OnLocationLayerClickListener {

  void onLocationLayerClick();
}
