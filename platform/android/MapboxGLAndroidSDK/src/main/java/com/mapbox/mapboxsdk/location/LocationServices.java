package com.mapbox.mapboxsdk.location;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.location.Location;
import android.location.LocationManager;
import android.support.annotation.NonNull;
import android.support.v4.content.ContextCompat;
import android.util.Log;

import com.mapbox.mapboxsdk.telemetry.TelemetryLocationReceiver;
import com.mapzen.android.lost.api.LocationRequest;
import com.mapzen.android.lost.api.LostApiClient;

import java.util.ArrayList;
import java.util.List;

/**
 * Manages locational updates. Contains methods to register and unregister location listeners.
 */
public class LocationServices implements com.mapzen.android.lost.api.LocationListener {

    private static final String TAG = "LocationServices";

    private static LocationServices instance;

    private Context context;
    private LostApiClient locationClient;
    private Location lastLocation;

    private List<LocationListener> locationListeners;

    private boolean isGPSEnabled;

    /**
     * Private constructor for singleton LocationServices
     */
    private LocationServices(Context context) {
        super();
        this.context = context;
        // Setup location services
        locationClient = new LostApiClient.Builder(context).build();
        locationListeners = new ArrayList<>();
    }

    /**
     * Primary (singleton) access method for LocationServices
     *
     * @param context Context
     * @return LocationServices
     */
    public static LocationServices getLocationServices(@NonNull final Context context) {
        if (instance == null) {
            instance = new LocationServices(context.getApplicationContext());
        }
        return instance;
    }

    /**
     * Enabled / Disable GPS focused location tracking
     *
     * @param enableGPS true if GPS is to be enabled, false if GPS is to be disabled
     */
    public void toggleGPS(boolean enableGPS) {
        if ((ContextCompat.checkSelfPermission(context, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) &&
                (ContextCompat.checkSelfPermission(context, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED)) {
            Log.w(TAG, "Location Permissions Not Granted Yet.  Try again after requesting.");
            return;
        }

        // Disconnect
        if (locationClient.isConnected()) {
            // Disconnect first to ensure that the new requests are GPS
            com.mapzen.android.lost.api.LocationServices.FusedLocationApi.removeLocationUpdates(this);
            locationClient.disconnect();
        }

        // Setup Fresh
        locationClient.connect();
        Location lastLocation = com.mapzen.android.lost.api.LocationServices.FusedLocationApi.getLastLocation();
        if (lastLocation != null) {
            this.lastLocation = lastLocation;
        }

        LocationRequest locationRequest;

        if (enableGPS) {
            // LocationRequest Tuned for GPS
            locationRequest = LocationRequest.create()
                    .setFastestInterval(1000)
                    .setSmallestDisplacement(3.0f)
                    .setPriority(LocationRequest.PRIORITY_HIGH_ACCURACY);

            com.mapzen.android.lost.api.LocationServices.FusedLocationApi.requestLocationUpdates(locationRequest, this);
        } else {
            // LocationRequest Tuned for PASSIVE
            locationRequest = LocationRequest.create()
                    .setFastestInterval(1000)
                    .setSmallestDisplacement(3.0f)
                    .setPriority(LocationRequest.PRIORITY_NO_POWER);

            com.mapzen.android.lost.api.LocationServices.FusedLocationApi.requestLocationUpdates(locationRequest, this);
        }

        isGPSEnabled = enableGPS;
    }

    /**
     * Returns if the GPS sensor is currently enabled
     *
     * @return active state of the GPS
     */
    public boolean isGPSEnabled() {
        return isGPSEnabled;
    }

    /**
     * Called when the location has changed.
     *
     * @param location The updated location
     */
    @Override
    public void onLocationChanged(Location location) {
//        Log.d(TAG, "onLocationChanged()..." + location);
        this.lastLocation = location;

        // Update Listeners
        for (LocationListener listener : this.locationListeners) {
            listener.onLocationChanged(location);
        }

        // Update the Telemetry Receiver
        Intent locIntent = new Intent(TelemetryLocationReceiver.INTENT_STRING);
        locIntent.putExtra(LocationManager.KEY_LOCATION_CHANGED, location);
        context.sendBroadcast(locIntent);
    }

    /**
     * Last known location
     *
     * @return Last known location data
     */
    public Location getLastLocation() {
        return lastLocation;
    }

    /**
     * Registers a LocationListener to receive location updates
     *
     * @param locationListener LocationListener
     */
    public void addLocationListener(@NonNull LocationListener locationListener) {
        if (!this.locationListeners.contains(locationListener)) {
            this.locationListeners.add(locationListener);
        }
    }

    /**
     * Unregister a LocationListener to stop receiving location updates
     *
     * @param locationListener LocationListener to remove
     * @return True if LocationListener was found and removed, False if it was not
     */
    public boolean removeLocationListener(@NonNull LocationListener locationListener) {
        return this.locationListeners.remove(locationListener);
    }
}
