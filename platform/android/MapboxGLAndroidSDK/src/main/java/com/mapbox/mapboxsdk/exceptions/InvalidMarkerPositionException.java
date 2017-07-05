package com.mapbox.mapboxsdk.exceptions;

/**
 * A InvalidMarkerPositionException is thrown when a Marker object is created with an invalid LatLng position.
 */
public class InvalidMarkerPositionException extends RuntimeException {

  /**
   * Creates a invalid marker position exception thrown when a Marker object is created with an invalid LatLng position.
   */
  public InvalidMarkerPositionException() {
    super("Adding an invalid Marker to a Map. "
      + "Missing the required position field. "
      + "Provide a non null LatLng as position to the Marker.");
  }
}
