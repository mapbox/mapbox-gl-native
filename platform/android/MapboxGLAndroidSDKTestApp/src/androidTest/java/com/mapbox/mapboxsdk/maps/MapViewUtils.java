package com.mapbox.mapboxsdk.maps;

import com.mapbox.mapboxsdk.geometry.LatLng;

/**
 * Utility class to bypass package visibility
 */
public class MapViewUtils {

  public static void setDirection(MapboxMap mapboxMap, float direction) {
    mapboxMap.getTransform().setBearing(direction);
  }

  public static float getDirection(MapboxMap mapboxMap) {
    return (float) mapboxMap.getTransform().getBearing();
  }

  public static void setTilt(MapboxMap mapboxMap, float tilt) {
    mapboxMap.getTransform().setTilt((double) tilt);
  }

  public static float getTilt(MapboxMap mapboxMap) {
    return (float) mapboxMap.getTransform().getTilt();
  }

  public static void setLatLng(MapboxMap mapboxMap, LatLng latLng) {
    mapboxMap.getTransform().setCenterCoordinate(latLng);
  }

  public static LatLng getLatLng(MapboxMap mapboxMap) {
    return mapboxMap.getTransform().getCenterCoordinate();
  }
}
