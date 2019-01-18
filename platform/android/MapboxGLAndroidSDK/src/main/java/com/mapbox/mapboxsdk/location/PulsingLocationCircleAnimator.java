package com.mapbox.mapboxsdk.location;

import android.view.animation.AccelerateInterpolator;
import android.view.animation.BounceInterpolator;
import android.view.animation.DecelerateInterpolator;
import android.view.animation.Interpolator;
import android.view.animation.LinearInterpolator;

import com.mapbox.mapboxsdk.location.modes.PulseMode;

/**
 * Manages the logic of the interpolated animation which is applied to the LocationComponent's pulsing circle
 */
public class PulsingLocationCircleAnimator extends MapboxFloatAnimator {

  /**
   *
   * @param updateListener  the {@link AnimationsValueChangeListener} associated with this animator.
   * @param maxAnimationFps the maximum frames per second that the animator should use. Default
   *                        is the {@link LocationAnimatorCoordinator#maxAnimationFps} variable.
   */
  public PulsingLocationCircleAnimator(AnimationsValueChangeListener updateListener,
                                       int maxAnimationFps,
                                       float circleMaxRadius) {
    super(0f, circleMaxRadius, updateListener, maxAnimationFps);
  }

  public Interpolator retrievePulseInterpolator(String desiredInterpolatorFromOptions) {
    switch (desiredInterpolatorFromOptions) {
      case PulseMode.LINEAR:
        return new LinearInterpolator();
      case PulseMode.ACCELERATE:
        return new AccelerateInterpolator();
      case PulseMode.DECELERATE:
        return new DecelerateInterpolator();
      case PulseMode.BOUNCE:
        return new BounceInterpolator();
      default:
        return new DecelerateInterpolator();
    }
  }
}