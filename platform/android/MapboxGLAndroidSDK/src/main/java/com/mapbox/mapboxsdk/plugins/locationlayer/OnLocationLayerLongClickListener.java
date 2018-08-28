package com.mapbox.mapboxsdk.plugins.locationlayer;

/**
 * The Location Layer Plugin exposes an API for listening to when the user long clicks on the location
 * layer icon visible on the map. when this event occurs, the {@link #onLocationLayerLongClick()} method
 * gets invoked.
 */
public interface OnLocationLayerLongClickListener {

  void onLocationLayerLongClick();
}
