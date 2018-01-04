package com.mapbox.mapboxsdk;


import android.location.Location;
import android.support.annotation.Nullable;

import com.mapbox.mapboxsdk.location.LocationSource;
import com.mapbox.services.android.telemetry.location.LocationEngine;
import com.mapbox.services.android.telemetry.location.LocationEngineListener;

class EmptyLocationSource extends LocationSource {

  /**
   * Activate the location engine which will connect whichever location provider you are using. You'll need to call
   * this before requesting user location updates using {@link LocationEngine#requestLocationUpdates()}.
   */
  @Override
  public void activate() {
    // Intentionally left empty
  }

  /**
   * Disconnect the location engine which is useful when you no longer need location updates or requesting the users
   * {@link LocationEngine#getLastLocation()}. Before deactivating, you'll need to stop request user location updates
   * using {@link LocationEngine#removeLocationUpdates()}.
   */
  @Override
  public void deactivate() {
    // Intentionally left empty
  }

  /**
   * Check if your location provider has been activated/connected. This is mainly used internally but is also useful in
   * the rare case when you'd like to know if your location engine is connected or not.
   *
   * @return boolean true if the location engine has been activated/connected, else false.
   */
  @Override
  public boolean isConnected() {
    return false;
  }

  /**
   * Returns the Last known location is the location provider is connected and location permissions are granted.
   *
   * @return the last known location
   */
  @Override
  @Nullable
  public Location getLastLocation() {
    return null;
  }

  /**
   * Request location updates to the location provider.
   */
  @Override
  public void requestLocationUpdates() {
    // Intentionally left empty
  }

  /**
   * Dismiss ongoing location update to the location provider.
   */
  @Override
  public void removeLocationUpdates() {
    // Intentionally left empty
  }

  /**
   * Invoked when the Location has changed.
   *
   * @param location the new location
   */
  @Override
  public void onLocationChanged(Location location) {
    // Intentionally left empty
  }

  /**
   * Useful when you'd like to add a location listener to handle location connections and update events. It is important
   * to note, that the callback will continue getting called even when your application isn't in the foreground.
   * Therefore, it is a good idea to use {@link LocationEngine#removeLocationEngineListener(LocationEngineListener)}
   * inside your activities {@code onStop()} method.
   *
   * @param listener A {@link LocationEngineListener} which you'd like to add to your location engine.
   * @since 2.0.0
   */
  @Override
  public void addLocationEngineListener(LocationEngineListener listener) {
    // Intentionally left empty
  }

  /**
   * If you no longer need your {@link LocationEngineListener} to be invoked with every location update, use this
   * method to remove it. It's also important to remove your listeners before the activity is destroyed to prevent any
   * potential memory leaks.
   *
   * @param listener the {@link LocationEngineListener} you'd like to remove from this {@link LocationEngine}.
   * @return true.
   * @since 2.0.0
   */
  @Override
  public boolean removeLocationEngineListener(LocationEngineListener listener) {
    return true;
  }
}
