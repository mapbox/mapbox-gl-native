package com.mapbox.mapboxsdk.constants;

/**
 * Contains constants used throughout the sdk classes.
 *
 * @since 6.0.0
 */
public class GeometryConstants {

  /**
   * The <a href='http://en.wikipedia.org/wiki/Earth_radius#Equatorial_radius'>equatorial radius</a>
   * value in meters
   *
   * @since 6.0.0
   */
  public static final int RADIUS_EARTH_METERS = 6378137;

  /**
   * This constant represents the lowest longitude value available to represent a geolocation.
   *
   * @since 6.0.0
   */
  public static final double MIN_LONGITUDE = -180;

  /**
   * This constant represents the highest longitude value available to represent a geolocation.
   *
   * @since 6.0.0
   */
  public static final double MAX_LONGITUDE = 180;

  /**
   * This constant represents the lowest latitude value available to represent a geolocation.
   *
   * @since 6.0.0
   */
  public static final double MIN_LATITUDE = -90;

  /**
   * This constant represents the latitude span when representing a geolocation.
   *
   * @since 6.0.0
   */
  public static final double LATITUDE_SPAN = 180;

  /**
   * This constant represents the longitude span when representing a geolocation.
   *
   * @since 6.0.0
   */
  public static final double LONGITUDE_SPAN = 360;

  /**
   * This constant represents the highest latitude value available to represent a geolocation.
   *
   * @since 6.0.0
   */
  public static final double MAX_LATITUDE = 90;

  /**
   * Maximum latitude value in Mercator projection.
   *
   * @since 6.0.0
   */
  public static final double MAX_MERCATOR_LATITUDE = 85.05112877980659;

  /**
   * Minimum latitude value in Mercator projection.
   *
   * @since 6.0.0
   */
  public static final double MIN_MERCATOR_LATITUDE = -85.05112877980659;

  private GeometryConstants() {
    // Private constructor to prevent initializing of this class.
  }
}
