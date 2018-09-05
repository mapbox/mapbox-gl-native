package com.mapbox.mapboxsdk.location.modes;

import android.location.Location;
import android.support.annotation.IntDef;

import com.mapbox.mapboxsdk.location.LocationComponent;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * Contains the variety of camera modes which determine how the camera will track
 * the user location.
 */
public final class CameraMode {

  private CameraMode() {
    // Class should not be initialized
  }

  /**
   * Determine the camera tracking behavior in the {@link LocationComponent}.
   */
  @IntDef( {NONE, NONE_COMPASS, NONE_GPS, TRACKING, TRACKING_COMPASS, TRACKING_GPS, TRACKING_GPS_NORTH})
  @Retention(RetentionPolicy.SOURCE)
  public @interface Mode {
  }

  /**
   * No camera tracking.
   */
  public static final int NONE = 0x00000008;

  /**
   * Camera does not track location, but does track compass bearing.
   */
  public static final int NONE_COMPASS = 0x00000010;

  /**
   * Camera does not track location, but does track GPS {@link Location} bearing.
   */
  public static final int NONE_GPS = 0x00000016;

  /**
   * Camera tracks the user location.
   */
  public static final int TRACKING = 0x00000018;

  /**
   * Camera tracks the user location, with bearing
   * provided by a compass.
   */
  public static final int TRACKING_COMPASS = 0x00000020;

  /**
   * Camera tracks the user location, with bearing
   * provided by a normalized {@link Location#getBearing()}.
   */
  public static final int TRACKING_GPS = 0x00000022;

  /**
   * Camera tracks the user location, with bearing
   * always set to north (0).
   */
  public static final int TRACKING_GPS_NORTH = 0x00000024;
}
