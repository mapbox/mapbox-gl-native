package com.mapbox.mapboxsdk.constants;

/**
 * GeoConstants exposes constants for doing locational calculations on Earth
 */
public class GeoConstants {

  /**
   * The <a href='http://en.wikipedia.org/wiki/Earth_radius#Equatorial_radius'>equatorial radius</a>
   * value in meters
   */
  public static final int RADIUS_EARTH_METERS = 6378137;

  /**
   * The minimum latitude on Earth. This is the minimum latitude representable
   * by Mapbox GL's Mercator projection, because the projection distorts latitude
   * near the poles towards infinity.
   */
  public static final double MIN_LATITUDE = -85.05112878;

  /**
   * The maximum latitude on Earth. This is the maximum latitude representable
   * by Mapbox GL's Mercator projection, because the projection distorts latitude
   * near the poles towards infinity.
   */
  public static final double MAX_LATITUDE = 85.05112878;

  /**
   * The minimum longitude on Earth
   */
  public static final double MIN_LONGITUDE = -180;

  /**
   * The maximum longitude on Earth
   */
  public static final double MAX_LONGITUDE = 180;

}
