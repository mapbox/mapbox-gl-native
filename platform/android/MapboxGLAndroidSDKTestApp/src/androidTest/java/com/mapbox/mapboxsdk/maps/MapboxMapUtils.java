package com.mapbox.mapboxsdk.maps;

/**
 * Utility class to fetch a MapboxMap instance by bypassing package visibility
 */
public class MapboxMapUtils {

    public static void setDirection(MapView mapView, float direction) {
        mapView.setBearing(direction);
    }

    public static void setTilt(MapView mapView, float tilt) {
        mapView.setTilt((double) tilt);
    }
}
