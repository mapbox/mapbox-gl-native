package com.mapbox.mapboxsdk.location;

import android.content.Context;
import android.location.Location;
import android.util.Log;

import com.mapbox.services.android.telemetry.location.LocationEngine;
import com.mapbox.services.android.telemetry.location.LocationEngineListener;
import com.mapbox.services.android.telemetry.location.LocationEnginePriority;
import com.mapbox.services.android.telemetry.permissions.PermissionsManager;
import com.mapzen.android.lost.api.LocationListener;
import com.mapzen.android.lost.api.LocationRequest;
import com.mapzen.android.lost.api.LocationServices;
import com.mapzen.android.lost.api.LostApiClient;

/**
 * Manages locational updates. Contains methods to register and unregister location listeners.
 * <ul>
 * <li>You can register a {@link LocationEngineListener} with
 * {@link #addLocationEngineListener(LocationEngineListener)} to receive
 * location updates.</li>
 * <li> You can unregister a {@link LocationEngineListener} with
 * {@link #removeLocationEngineListener(LocationEngineListener)} to stop receiving location updates.</li>
 * </ul>
 * <p>
 * Note: If registering a listener in your Activity.onStart() implementation, you should unregister it in
 * Activity.onStop(). (You won't receive location updates when paused, and this will cut down on unnecessary system
 * overhead). Do not unregister in Activity.onSaveInstanceState(), because this won't be called if the user moves back
 * in the history stack.
 * </p>
 */
public class LocationSource extends LocationEngine implements
  LostApiClient.ConnectionCallbacks, LocationListener {

  private static final String LOG_TAG = LocationSource.class.getSimpleName();

  private static LocationEngine instance;

  private Context context;
  private LostApiClient lostApiClient;

  public LocationSource(Context context) {
    super();
    this.context = context;
    lostApiClient = new LostApiClient.Builder(context)
      .addConnectionCallbacks(this)
      .build();
  }

  public static synchronized LocationEngine getLocationEngine(Context context) {
    if (instance == null) {
      instance = new LocationSource(context.getApplicationContext());
    }

    return instance;
  }

  @Override
  public void activate() {
    if (lostApiClient != null && !lostApiClient.isConnected()) {
      lostApiClient.connect();
    }
  }

  @Override
  public void deactivate() {
    if (lostApiClient != null && lostApiClient.isConnected()) {
      lostApiClient.disconnect();
    }
  }

  @Override
  public boolean isConnected() {
    return lostApiClient.isConnected();
  }

  @Override
  public void onConnected() {
    for (LocationEngineListener listener : locationListeners) {
      listener.onConnected();
    }
  }

  @Override
  public void onConnectionSuspended() {
    Log.d(LOG_TAG, "Connection suspended.");
  }

  @Override
  public Location getLastLocation() {
    if (lostApiClient.isConnected() && PermissionsManager.areLocationPermissionsGranted(context)) {
      //noinspection MissingPermission
      return LocationServices.FusedLocationApi.getLastLocation(lostApiClient);
    }

    return null;
  }

  @Override
  public void requestLocationUpdates() {
    // Common params
    LocationRequest request = LocationRequest.create()
      .setFastestInterval(1000)
      .setSmallestDisplacement(3.0f);

    // Priority matching is straightforward
    if (priority == LocationEnginePriority.NO_POWER) {
      request.setPriority(LocationRequest.PRIORITY_NO_POWER);
    } else if (priority == LocationEnginePriority.LOW_POWER) {
      request.setPriority(LocationRequest.PRIORITY_LOW_POWER);
    } else if (priority == LocationEnginePriority.BALANCED_POWER_ACCURACY) {
      request.setPriority(LocationRequest.PRIORITY_BALANCED_POWER_ACCURACY);
    } else if (priority == LocationEnginePriority.HIGH_ACCURACY) {
      request.setPriority(LocationRequest.PRIORITY_HIGH_ACCURACY);
    }

    if (lostApiClient.isConnected() && PermissionsManager.areLocationPermissionsGranted(context)) {
      //noinspection MissingPermission
      LocationServices.FusedLocationApi.requestLocationUpdates(lostApiClient, request, this);
    }
  }

  @Override
  public void removeLocationUpdates() {
    LocationServices.FusedLocationApi.removeLocationUpdates(lostApiClient, this);
  }

  @Override
  public void onLocationChanged(Location location) {
    for (LocationEngineListener listener : locationListeners) {
      listener.onLocationChanged(location);
    }
  }

  @Override
  public void onProviderDisabled(String provider) {
    Log.d(LOG_TAG, "Provider disabled: " + provider);
  }

  @Override
  public void onProviderEnabled(String provider) {
    Log.d(LOG_TAG, "Provider enabled: " + provider);
  }
}
