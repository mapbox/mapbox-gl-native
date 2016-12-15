package com.mapbox.mapboxsdk.exceptions;

/**
 * A InvalidLatLngBoundsException is thrown by {@link com.mapbox.mapboxsdk.geometry.LatLngBounds}
 * when there aren't enough {@link com.mapbox.mapboxsdk.geometry.LatLng} to create a bounds.
 */
public class InvalidLatLngBoundsException extends RuntimeException {

  public InvalidLatLngBoundsException(int latLngsListSize) {
    super("Cannot create a LatLngBounds from " + latLngsListSize + " items");
  }
}
