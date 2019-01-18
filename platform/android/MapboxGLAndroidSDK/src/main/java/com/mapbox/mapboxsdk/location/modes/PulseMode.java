package com.mapbox.mapboxsdk.location.modes;

import android.support.annotation.StringDef;

import com.mapbox.mapboxsdk.location.LocationComponentOptions;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

public final class PulseMode {

  private PulseMode() {
    // Class should not be initialized
  }

  /**
   * An interpolator defines the rate of change of an animation.
   *
   * One of these constants should be used with {@link LocationComponentOptions#pulseInterpolator}.
   *
   */
  @StringDef( {LINEAR, ACCELERATE, DECELERATE, BOUNCE})
  @Retention(RetentionPolicy.SOURCE)
  public @interface Mode {
  }

  /**
   * An interpolator where the rate of change is constant.
   */
  public static final String LINEAR = "linear";

  /**
   * An interpolator where the rate of change starts out slowly and and then accelerates.
   */
  public static final String ACCELERATE = "accelerate";

  /**
   * An interpolator where the rate of change starts out quickly and and then decelerates.
   */
  public static final String DECELERATE = "decelerate";

  /**
   * An interpolator where the change bounces at the end.
   */
  public static final String BOUNCE = "bounce";
}
