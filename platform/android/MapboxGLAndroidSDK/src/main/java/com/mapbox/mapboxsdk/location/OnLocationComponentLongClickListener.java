package com.mapbox.mapboxsdk.location;

/**
 * The Location Layer Plugin exposes an API for listening to when the user long clicks on the location
 * layer icon visible on the map. when this event occurs, the {@link #onLocationComponentLongClick()} method
 * gets invoked.
 */
public interface OnLocationComponentLongClickListener {

  void onLocationComponentLongClick();
}
