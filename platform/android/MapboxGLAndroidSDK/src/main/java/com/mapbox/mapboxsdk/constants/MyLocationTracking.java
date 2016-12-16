package com.mapbox.mapboxsdk.constants;

import android.support.annotation.IntDef;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationView;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * MyLocationTracking exposes different types of locational tracking modes.
 *
 * @see com.mapbox.mapboxsdk.maps.TrackingSettings#setMyLocationTrackingMode(int)
 * @see MyLocationView#setMyLocationTrackingMode(int)
 */
public class MyLocationTracking {

  /**
   * Indicates the parameter accepts one of the values from {@link MyLocationTracking}.
   */
  @IntDef( {TRACKING_NONE, TRACKING_FOLLOW})
  @Retention(RetentionPolicy.SOURCE)
  public @interface Mode {
  }

  /**
   * Location tracking is disabled.
   */
  public static final int TRACKING_NONE = 0x00000000;

  /**
   * Tracking the location of the user, {@link MapView} will reposition to center of {@link MyLocationView}
   */
  public static final int TRACKING_FOLLOW = 0x00000004;

}