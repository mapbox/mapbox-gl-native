package com.mapbox.mapboxsdk.location;

import android.location.Location;

public interface LocationListener {

    /**
     * Callback method for receiving location updates from LocationServices.
     * @param location The new Location data
     */
    public void onLocationChanged(Location location);

}
