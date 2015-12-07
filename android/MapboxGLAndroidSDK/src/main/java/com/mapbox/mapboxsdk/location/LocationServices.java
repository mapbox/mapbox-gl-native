package com.mapbox.mapboxsdk.location;

import android.content.Context;
import android.support.annotation.NonNull;
import com.mapzen.android.lost.api.LocationRequest;
import com.mapzen.android.lost.api.LostApiClient;

public class LocationServices {

    private static LocationServices instance = null;

    private LostApiClient mLocationClient;
    private LocationRequest mLocationRequest;


    /**
     * Private constructor for singleton LocationServices
     */
    private LocationServices(Context context) {
        super();
        // Setup location services
        mLocationClient = new LostApiClient.Builder(context).build();
        mLocationRequest = LocationRequest.create()
                .setFastestInterval(1000)
                .setSmallestDisplacement(3.0f)
                .setPriority(LocationRequest.PRIORITY_HIGH_ACCURACY);
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
}
