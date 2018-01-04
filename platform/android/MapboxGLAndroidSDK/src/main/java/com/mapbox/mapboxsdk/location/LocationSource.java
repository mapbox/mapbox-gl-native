package com.mapbox.mapboxsdk.location;

import android.content.Context;
import android.location.Location;
import android.support.annotation.Nullable;

import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.services.android.telemetry.location.LocationEngine;
import com.mapbox.services.android.telemetry.location.LocationEngineListener;
import com.mapbox.services.android.telemetry.location.LocationEnginePriority;
import com.mapzen.android.lost.api.LocationListener;
import com.mapzen.android.lost.api.LocationRequest;
import com.mapzen.android.lost.api.LocationServices;
import com.mapzen.android.lost.api.LostApiClient;

/**
 * LocationEngine using the Open Source Lost library
 * Manages locational updates. Contains methods to register and unregister location listeners.
 * <ul>
 * <li>You can register a LocationEngineListener with LocationSource#addLocationEngineListener(LocationEngineListener)
 * to receive location updates.</li>
 * <li> You can unregister a LocationEngineListener with
 * LocationEngine#removeLocationEngineListener(LocationEngineListener)} to stop receiving location updates.</li>
 * </ul>
 * <p>
 * Note: If registering a listener in your Activity.onStart() implementation, you should unregister it in
 * Activity.onStop(). (You won't receive location updates when paused, and this will cut down on unnecessary system
 * overhead). Do not unregister in Activity.onSaveInstanceState(), because this won't be called if the user moves back
 * in the history stack.
 * </p>
 *
 * @deprecated Use a {@link Mapbox#getLocationEngine()} instead.
 */
@Deprecated
public class LocationSource extends LocationEngine implements LostApiClient.ConnectionCallbacks, LocationListener {

  private Context context;
  private LostApiClient lostApiClient;

  /**
   * Constructs a location source instance.
   *
   * @param context the context from which the Application context will be derived.
   */
  public LocationSource(Context context) {
    super();
    this.context = context.getApplicationContext();
    lostApiClient = new LostApiClient.Builder(this.context)
      .addConnectionCallbacks(this)
      .build();
  }

  /**
   * Constructs a location source instance.
   * Needed to create empty location source implementations.
   */
  public LocationSource() {
  }

  /**
   * Activate the location engine which will connect whichever location provider you are using. You'll need to call
   * this before requesting user location updates using {@link LocationEngine#requestLocationUpdates()}.
   */
  @Override
  public void activate() {
    connect();
  }

  /**
   * Disconnect the location engine which is useful when you no longer need location updates or requesting the users
   * {@link LocationEngine#getLastLocation()}. Before deactivating, you'll need to stop request user location updates
   * using {@link LocationEngine#removeLocationUpdates()}.
   */
  @Override
  public void deactivate() {
    if (lostApiClient != null && lostApiClient.isConnected()) {
      lostApiClient.disconnect();
    }
  }

  /**
   * Check if your location provider has been activated/connected. This is mainly used internally but is also useful in
   * the rare case when you'd like to know if your location engine is connected or not.
   *
   * @return boolean true if the location engine has been activated/connected, else false.
   */
  @Override
  public boolean isConnected() {
    return lostApiClient.isConnected();
  }

  /**
   * Invoked when the location provider has connected.
   */
  @Override
  public void onConnected() {
    for (LocationEngineListener listener : locationListeners) {
      listener.onConnected();
    }
  }

  /**
   * Invoked when the location provider connection has been suspended.
   */
  @Override
  public void onConnectionSuspended() {
    // Empty
  }

  /**
   * Returns the Last known location is the location provider is connected and location permissions are granted.
   *
   * @return the last known location
   */
  @Override
  @Nullable
  public Location getLastLocation() {
    if (lostApiClient.isConnected()) {
      //noinspection MissingPermission
      return LocationServices.FusedLocationApi.getLastLocation(lostApiClient);
    }
    return null;
  }

  /**
   * Request location updates to the location provider.
   */
  @Override
  public void requestLocationUpdates() {
    LocationRequest request = LocationRequest.create();

    if (interval != null) {
      request.setInterval(interval);
    }
    if (fastestInterval != null) {
      request.setFastestInterval(fastestInterval);
    }
    if (smallestDisplacement != null) {
      request.setSmallestDisplacement(smallestDisplacement);
    }

    if (priority == LocationEnginePriority.NO_POWER) {
      request.setPriority(LocationRequest.PRIORITY_NO_POWER);
    } else if (priority == LocationEnginePriority.LOW_POWER) {
      request.setPriority(LocationRequest.PRIORITY_LOW_POWER);
    } else if (priority == LocationEnginePriority.BALANCED_POWER_ACCURACY) {
      request.setPriority(LocationRequest.PRIORITY_BALANCED_POWER_ACCURACY);
    } else if (priority == LocationEnginePriority.HIGH_ACCURACY) {
      request.setPriority(LocationRequest.PRIORITY_HIGH_ACCURACY);
    }

    if (lostApiClient.isConnected()) {
      //noinspection MissingPermission
      LocationServices.FusedLocationApi.requestLocationUpdates(lostApiClient, request, this);
    }
  }

  /**
   * Dismiss ongoing location update to the location provider.
   */
  @Override
  public void removeLocationUpdates() {
    if (lostApiClient.isConnected()) {
      LocationServices.FusedLocationApi.removeLocationUpdates(lostApiClient, this);
    }
  }

  /**
   * Returns the location engine type.
   *
   * @return Lost type
   */
  @Override
  public Type obtainType() {
    return Type.LOST;
  }

  /**
   * Invoked when the Location has changed.
   *
   * @param location the new location
   */
  @Override
  public void onLocationChanged(Location location) {
    for (LocationEngineListener listener : locationListeners) {
      listener.onLocationChanged(location);
    }
  }

  private void connect() {
    if (lostApiClient != null) {
      if (lostApiClient.isConnected()) {
        onConnected();
      } else {
        lostApiClient.connect();
      }
    }
  }
}