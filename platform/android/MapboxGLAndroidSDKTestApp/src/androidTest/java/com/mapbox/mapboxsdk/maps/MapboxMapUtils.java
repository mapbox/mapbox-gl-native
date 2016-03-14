package com.mapbox.mapboxsdk.maps;

/**
 * Utility class to fetch a MapboxMap instance by bypassing package visibility
 */
public class MapboxMapUtils {

    /**
     * Get the MapboxMap instance linked to passed MapView
     *
     * @param mapView
     * @return
     */
    public static MapboxMap getMapboxMap(MapView mapView) {
        return mapView.getMapboxMap();
    }

    /**
     * Set the direction of the user
     *
     * @param mapView
     * @param direction
     */
    public static void setDirection(MapView mapView, double direction) {
        mapView.setDirection(direction);
    }

}
