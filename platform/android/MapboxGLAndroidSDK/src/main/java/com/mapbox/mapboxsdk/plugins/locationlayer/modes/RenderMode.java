package com.mapbox.mapboxsdk.plugins.locationlayer.modes;

import android.support.annotation.IntDef;

import com.mapbox.mapboxsdk.plugins.locationlayer.CompassEngine;
import com.mapbox.mapboxsdk.plugins.locationlayer.LocationLayerPlugin;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * Contains the variety of ways the user location can be rendered on the map.
 */
public final class RenderMode {

  private RenderMode() {
    // Class should not be initialized
  }

  /**
   * One of these constants should be used with {@link LocationLayerPlugin#setRenderMode(int)}.
   * mode can be switched at anytime by calling the {@code setLocationLayerMode} method passing
   * in the new mode you'd like the location layer to be in.
   */
  @IntDef( {COMPASS, GPS, NORMAL})
  @Retention(RetentionPolicy.SOURCE)
  public @interface Mode {
  }

  /**
   * Basic tracking is enabled, bearing ignored.
   */
  public static final int NORMAL = 0x00000012;

  /**
   * Tracking the user location with bearing considered
   * from a {@link CompassEngine}.
   */
  public static final int COMPASS = 0x00000004;

  /**
   * Tracking the user location with bearing considered from {@link android.location.Location}.
   */
  public static final int GPS = 0x00000008;
}
