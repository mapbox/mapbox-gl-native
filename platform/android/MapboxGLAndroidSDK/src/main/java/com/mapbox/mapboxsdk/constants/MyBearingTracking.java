package com.mapbox.mapboxsdk.constants;

import android.support.annotation.IntDef;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationView;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * MyBearingTracking exposes different types of bearing tracking modes.
 * <p>
 * These modes visualise the user direction by extracting the direction from either sensor or location data.
 * </p>
 * <p>
 * Required to enable showing the user location first through {@link MapboxMap#setMyLocationEnabled(boolean)}.
 * </p>
 *
 * @see com.mapbox.mapboxsdk.maps.TrackingSettings#setMyBearingTrackingMode(int)
 * @see MyLocationView#setMyBearingTrackingMode(int)
 */
public class MyBearingTracking {

  @IntDef( {NONE, COMPASS, GPS, GPS_NORTH_FACING})
  @Retention(RetentionPolicy.SOURCE)
  public @interface Mode {
  }

  /**
   * Bearing tracking is disabled
   */
  public static final int NONE = 0x00000000;

  /**
   * Tracking the bearing of the user based on sensor data
   */
  public static final int COMPASS = 0x00000004;

  /**
   * Tracking the bearing of the user based on GPS data
   */
  public static final int GPS = 0x00000008;

  /**
   * Tracking the bearing of the user based on GPS data, but camera always faces north direction
   */
  public static final int GPS_NORTH_FACING = 0x0000000B;

}
