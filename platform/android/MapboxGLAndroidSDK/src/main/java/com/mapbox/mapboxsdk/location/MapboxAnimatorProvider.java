package com.mapbox.mapboxsdk.location;

import android.animation.ValueAnimator;
import android.support.annotation.Nullable;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;

final class MapboxAnimatorProvider {

  private static MapboxAnimatorProvider INSTANCE;

  private MapboxAnimatorProvider() {
    // private constructor
  }

  public static MapboxAnimatorProvider getInstance() {
    if (INSTANCE == null) {
      INSTANCE = new MapboxAnimatorProvider();
    }
    return INSTANCE;
  }

  MapboxLatLngAnimator latLngAnimator(LatLng previous, LatLng target,
                                      MapboxAnimator.AnimationsValueChangeListener updateListener,
                                      int maxAnimationFps) {
    return new MapboxLatLngAnimator(previous, target, updateListener, maxAnimationFps);
  }

  MapboxFloatAnimator floatAnimator(Float previous, Float target,
                                    MapboxAnimator.AnimationsValueChangeListener updateListener,
                                    int maxAnimationFps) {
    return new MapboxFloatAnimator(previous, target, updateListener, maxAnimationFps);
  }

  MapboxCameraAnimatorAdapter cameraAnimator(Float previous, Float target,
                                             MapboxAnimator.AnimationsValueChangeListener updateListener,
                                             @Nullable MapboxMap.CancelableCallback cancelableCallback) {
    return new MapboxCameraAnimatorAdapter(previous, target, updateListener, cancelableCallback);
  }

  /**
   * This animator is for the LocationComponent pulsing circle.
   *
   * @param updateListener the listener that is found in the {@link LocationAnimatorCoordinator}'s
   *                       listener array.
   * @param maxAnimationFps the max frames per second of the pulsing animation
   * @param pulseSingleDuration the number of milliseconds it takes for the animator to create
   *                            a single pulse.
   * @param pulseMaxRadius the max radius when the circle is finished with a single pulse.
   * @param desiredInterpolatorFromOptions the type of Android-system interpolator to use for
   *                                       the pulsing animation (linear, accelerate, bounce, etc.)
   * @return a built {@link PulsingLocationCircleAnimator} object.
   */
  PulsingLocationCircleAnimator pulsingCircleAnimator(MapboxAnimator.AnimationsValueChangeListener updateListener,
                                                      int maxAnimationFps,
                                                      float pulseSingleDuration,
                                                      float pulseMaxRadius,
                                                      String desiredInterpolatorFromOptions) {
    PulsingLocationCircleAnimator pulsingLocationCircleAnimator =
        new PulsingLocationCircleAnimator(updateListener, maxAnimationFps, pulseMaxRadius);
    pulsingLocationCircleAnimator.setDuration((long) pulseSingleDuration);
    pulsingLocationCircleAnimator.setRepeatMode(ValueAnimator.RESTART);
    pulsingLocationCircleAnimator.setRepeatCount(ValueAnimator.INFINITE);
    pulsingLocationCircleAnimator.retrievePulseInterpolator(desiredInterpolatorFromOptions);
    pulsingLocationCircleAnimator.setInterpolator(
        pulsingLocationCircleAnimator.retrievePulseInterpolator(desiredInterpolatorFromOptions));
    return pulsingLocationCircleAnimator;
  }
}
