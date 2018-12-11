package com.mapbox.mapboxsdk.constants;

/**
 * Contains constants used throughout the sdk classes.
 */
public class GeometryConstants {

  /**
   * The <a href='http://en.wikipedia.org/wiki/Earth_radius#Equatorial_radius'>equatorial radius</a>
   * value in meters
   */
  public static final int RADIUS_EARTH_METERS = 6378137;

  /**
   * This constant represents the lowest longitude value available to represent a wrapped geolocation.
   */
  public static final double MIN_WRAP_LONGITUDE = -180;

  /**
   * This constant represents the highest longitude value available to represent a wrapped geolocation.
   */
  public static final double MAX_WRAP_LONGITUDE = 180;

  /**
   * This constant represents the lowest longitude value available to represent a geolocation.
   */
  public static final double MIN_LONGITUDE = -Double.MAX_VALUE;

  /**
   * This constant represents the highest longitude value available to represent a geolocation.
   */
  public static final double MAX_LONGITUDE = Double.MAX_VALUE;

  /**
   * This constant represents the lowest latitude value available to represent a geolocation.
   */
  public static final double MIN_LATITUDE = -90;

  /**
   * This constant represents the latitude span when representing a geolocation.
   */
  public static final double LATITUDE_SPAN = 180;

  /**
   * This constant represents the longitude span when representing a geolocation.
   */
  public static final double LONGITUDE_SPAN = 360;

  /**
   * This constant represents the highest latitude value available to represent a geolocation.
   */
  public static final double MAX_LATITUDE = 90;

  /**
   * Maximum latitude value in Mercator projection.
   */
  public static final double MAX_MERCATOR_LATITUDE = 85.05112877980659;

  /**
   * Minimum latitude value in Mercator projection.
   */
  public static final double MIN_MERCATOR_LATITUDE = -85.05112877980659;

  private GeometryConstants() {
    // Private constructor to prevent initializing of this class.
  }
}
