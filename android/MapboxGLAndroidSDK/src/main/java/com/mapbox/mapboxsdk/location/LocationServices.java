package com.mapbox.mapboxsdk.location;

public class LocationServices {

    private static LocationServices instance = null;

    /**
     * Private constructor for singleton LocationServices
     */
    private LocationServices() {
        super();
    }

    /**
     * Primary (singleton) access method for LocationServices
     * @return LocationServices
     */
    public static LocationServices getLocationServices() {
        if (instance == null) {
            instance = new LocationServices();
        }
        return instance;
    }
}
