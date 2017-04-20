package com.mapbox.mapboxsdk.testapp.activity.userlocation;


import android.location.Location;
import android.os.Handler;

import com.mapbox.services.android.telemetry.location.LocationEngine;
import com.mapbox.services.android.telemetry.location.LocationEngineListener;

/**
 * Sample LocationEngine that provides mocked locations simulating GPS updates
 */
public class MockLocationEngine extends LocationEngine {

  // Mocked data
  private static final int UPDATE_INTERVAL_MS = 1000;
  private static final double[][] locations = new double[][] {
    new double[] {39.489309, -0.360415},
    new double[] {39.492469, -0.358777},
    new double[] {40.393285, -3.707260},
    new double[] {40.394374, -3.707767},
    new double[] {40.398012, -3.715943},
    new double[] {40.416913, -3.703861}};

  private Handler handler;
  int currentIndex;

  public MockLocationEngine() {
    super();
  }

  @Override
  public void activate() {
    currentIndex = 0;

    // "Connection" is immediate here
    for (LocationEngineListener listener : locationListeners) {
      listener.onConnected();
    }
  }

  @Override
  public void deactivate() {
    handler = null;
  }

  @Override
  public boolean isConnected() {
    return true; // Always connected
  }

  @Override
  public Location getLastLocation() {
    return getNextLocation();
  }

  @Override
  public void requestLocationUpdates() {
    // Fake regular updates with a handler
    handler = new Handler();
    handler.postDelayed(new LocationUpdateRunnable(), UPDATE_INTERVAL_MS);
  }

  @Override
  public void removeLocationUpdates() {
    handler.removeCallbacksAndMessages(null);
  }

  private Location getNextLocation() {
    // Build the next location and rotate the index
    Location location = new Location(MockLocationEngine.class.getSimpleName());
    location.setLatitude(locations[currentIndex][0]);
    location.setLongitude(locations[currentIndex][1]);
    currentIndex = (currentIndex == locations.length - 1 ? 0 : currentIndex + 1);
    return location;
  }

  private class LocationUpdateRunnable implements Runnable {
    @Override
    public void run() {
      // Notify of an update
      Location location = getNextLocation();
      for (LocationEngineListener listener : locationListeners) {
        listener.onLocationChanged(location);
      }

      if (handler != null) {
        // Schedule the next update
        handler.postDelayed(new LocationUpdateRunnable(), UPDATE_INTERVAL_MS);
      }
    }
  }
}
