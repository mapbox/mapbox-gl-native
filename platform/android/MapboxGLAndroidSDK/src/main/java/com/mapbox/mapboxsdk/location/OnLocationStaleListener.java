package com.mapbox.mapboxsdk.location;

/**
 * Listener that can be added as a callback when the last location update
 * is considered stale.
 * <p>
 * The time from the last location update that determines if a location update
 * is stale or not is provided by {@link LocationComponentOptions#staleStateTimeout()}.
 */
public interface OnLocationStaleListener {

  void onStaleStateChange(boolean isStale);
}
