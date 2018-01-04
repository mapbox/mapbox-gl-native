package com.mapbox.mapboxsdk.constants;

import android.support.annotation.IntDef;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.TrackingSettings;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationView;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * MyLocationTracking exposes types of location tracking modes.
 * * <p>
 * This allows tracking the user location on screen by updating the camera position when a location update occurs.
 * </p>
 * <p>
 * Required to enable showing the user location first through {@link MapboxMap#setMyLocationEnabled(boolean)}.
 * </p>
 *
 * @see MapboxMap#setMyLocationEnabled(boolean)
 * @see TrackingSettings#setMyLocationTrackingMode(int)
 */
public class MyLocationTracking {

  @IntDef( {TRACKING_NONE, TRACKING_FOLLOW})
  @Retention(RetentionPolicy.SOURCE)
  public @interface Mode {
  }

  /**
   * Tracking the location of the user is disabled.
   */
  public static final int TRACKING_NONE = 0x00000000;

  /**
   * Tracking the location of the user. {@link MapView} will reposition to center of {@link MyLocationView}
   */
  public static final int TRACKING_FOLLOW = 0x00000004;

}
