package com.mapbox.mapboxsdk.location;

import android.content.Context;
import android.location.Location;
import android.support.annotation.NonNull;
import com.mapzen.android.lost.api.LocationRequest;
import com.mapzen.android.lost.api.LostApiClient;
import java.util.ArrayList;
import java.util.List;

public class LocationServices implements com.mapzen.android.lost.api.LocationListener {

    private static LocationServices instance = null;

    private LostApiClient mLocationClient;
    private LocationRequest mLocationRequest;

    private Location lastLocation = null;

    private List<LocationListener> locationListeners = null;

    private boolean isGPSEnabled = false;

    /**
     * Private constructor for singleton LocationServices
     */
    private LocationServices(Context context) {
        super();
        // Setup location services
        mLocationClient = new LostApiClient.Builder(context).build();
        locationListeners = new ArrayList<>();
    }

    /**
     * Primary (singleton) access method for LocationServices
     * @param context Context
     * @return LocationServices
     */
    public static LocationServices getLocationServices(@NonNull final Context context) {
        if (instance == null) {
            if (context == null) {
                throw new NullPointerException("Context required for accessing LocationServices");
            }
            instance = new LocationServices(context);
        }
        return instance;
    }

    /**
     * Enabled / Disable GPS focused location tracking
     *
     * @param enableGPS true if GPS is to be enabled, false if GPS is to be disabled
     */
    public void toggleGPS(boolean enableGPS) {

        if (enableGPS) {

            if (mLocationClient.isConnected()) {
                // Disconnect first to ensure that the new requests are GPS
                com.mapzen.android.lost.api.LocationServices.FusedLocationApi.removeLocationUpdates(this);
                mLocationClient.disconnect();
            }

            // Setup Fresh
            mLocationClient.connect();
            Location lastLocation = com.mapzen.android.lost.api.LocationServices.FusedLocationApi.getLastLocation();
            if (lastLocation != null) {
                this.lastLocation = lastLocation;
            }

            // LocationRequest Tuned for GPS
            mLocationRequest = LocationRequest.create()
                    .setFastestInterval(1000)
                    .setSmallestDisplacement(3.0f)
                    .setPriority(LocationRequest.PRIORITY_HIGH_ACCURACY);

            com.mapzen.android.lost.api.LocationServices.FusedLocationApi.requestLocationUpdates(mLocationRequest, this);

        } else {

            // Disconnect
            if (mLocationClient.isConnected()) {
                // Disconnect first to ensure that the new requests are GPS
                com.mapzen.android.lost.api.LocationServices.FusedLocationApi.removeLocationUpdates(this);
                mLocationClient.disconnect();
            }

        }

        isGPSEnabled = enableGPS;
    }

    public boolean isGPSEnabled() {
        return isGPSEnabled;
    }

    @Override
    public void onLocationChanged(Location location) {
        this.lastLocation = location;

        // Update Listeners
        for (LocationListener listener : this.locationListeners) {
            listener.onLocationChanged(location);
        }
    }

    /**
     * Last known location
     * @return Last known location data
     */
    public Location getLastLocation() {
        return lastLocation;
    }

    /**
     * Registers a LocationListener to receive location updates
     * @param locationListener LocationListener
     */
    public void addLocationListener(@NonNull LocationListener locationListener) {
        this.locationListeners.add(locationListener);
    }

    /**
     * Unregister a LocationListener to stop receiving location updates
     * @param locationListener LocationListener to remove
     * @return True if LocationListener was found and removed, False if it was not
     */
    public boolean removeLocationListener(@NonNull LocationListener locationListener) {
        return this.locationListeners.remove(locationListener);
    }
}
