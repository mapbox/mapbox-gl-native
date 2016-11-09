package com.mapbox.mapboxsdk.maps;

import com.mapbox.mapboxsdk.geometry.LatLng;

/**
 * Utility class to bypass package visibility
 */
public class MapViewUtils {

    public static void setDirection(MapView mapView, float direction) {
        mapView.setBearing(direction);
    }

    public static float getDirection(MapView mapView) {
        return (float) mapView.getDirection();
    }

    public static void setTilt(MapView mapView, float tilt) {
        mapView.setTilt((double) tilt);
    }

    public static float getTilt(MapView mapView) {
        return (float) mapView.getTilt();
    }

    public static void setLatLng(MapView mapView, LatLng latLng) {
        mapView.setCenterCoordinate(latLng);
    }

    public static LatLng getLatLng(MapView mapView) {
        return mapView.getCenterCoordinate();
    }

    public static int[] getContentPadding(MapView mapView) {
        return new int[]{mapView.getContentPaddingLeft(), mapView.getContentPaddingTop(), mapView.getContentPaddingRight(), mapView.getContentPaddingBottom()};
    }
}
