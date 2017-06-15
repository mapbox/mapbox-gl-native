package com.mapbox.mapboxsdk.exceptions;

/**
 * An InvalidLatLngBoundsException is thrown by LatLngBounds
 * when there aren't enough LatLng to create a bounds.
 */
public class InvalidLatLngBoundsException extends RuntimeException {

  public InvalidLatLngBoundsException(int latLngsListSize) {
    super("Cannot create a LatLngBounds from " + latLngsListSize + " items");
  }
}
