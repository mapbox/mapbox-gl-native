package com.mapbox.mapboxsdk.location;

import android.location.Location;

/**
 * Callback interface for when a location change occurs.
 */
public interface LocationListener {

  /**
   * Callback method for receiving location updates from LocationServices.
   *
   * @param location The new Location data
   */
  void onLocationChanged(Location location);

}
