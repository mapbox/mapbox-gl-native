package com.mapbox.mapboxsdk.constants;

import android.support.annotation.IntDef;

import com.mapbox.mapboxsdk.maps.widgets.MyLocationView;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * MyBearingTracking exposes different types bearing tracking modes.
 *
 * @see com.mapbox.mapboxsdk.maps.TrackingSettings#setMyBearingTrackingMode(int)
 * @see MyLocationView#setMyBearingTrackingMode(int)
 */
public class MyBearingTracking {

  /**
   * Indicates the parameter accepts one of the values from {@link MyBearingTracking}.
   */
  @IntDef( {NONE, COMPASS, GPS, /**COMBINED**/})
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

  //public static final int COMBINED = 0x00000012;

}
