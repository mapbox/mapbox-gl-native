package com.mapbox.mapboxsdk.geometry;

/**
 * Describes a latitude, longitude, and altitude tuple.
 */
public interface ILatLng {

  /**
   * Get the latitude, in degrees.
   *
   * @return the latitude value in degrees
   */
  double getLatitude();

  /**
   * Get the longitude, in degrees.
   *
   * @return the longitude value in degrees
   */
  double getLongitude();

  /**
   * Get the altitude, in meters.
   *
   * @return the altitude value in meters
   */
  double getAltitude();
}
