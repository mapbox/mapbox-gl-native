package com.mapbox.mapboxsdk.geometry;

/**
 * Describes a projection in Mercator meters.
 */
public interface IProjectedMeters {

  /**
   * Get the north projection, in meters.
   *
   * @return the projected meters in north direction
   */
  double getNorthing();

  /**
   * Get the east projection, in meters.
   *
   * @return the projected meters in east direction
   */
  double getEasting();

}
