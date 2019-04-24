package com.mapbox.mapboxsdk.location;

import android.animation.ValueAnimator;
import android.os.Build;
import android.util.Log;
import android.view.animation.Interpolator;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.Layer;

import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_PULSING_CIRCLE_LAYER;

/**
 * Manages the logic of the interpolated animation which is applied to the LocationComponent's pulsing circle
 */
public class PulsingLocationCircleAnimator extends MapboxFloatAnimator {

  /**
   * Start the LocationComponent circle pulse animation
   *
   * @param interpolatorToUse        the type of Android-system interpolator to use
   * @param mapboxMap                the MapboxMap object which pulsing circle should be shown on
   * @param locationComponentOptions the stying options of the LocationComponent pulsing circle
   */
  public PulsingLocationCircleAnimator(AnimationsValueChangeListener updateListener, int maxAnimationFps, LocationComponentOptions options) {
    super(0f, 60f, updateListener, maxAnimationFps);
    setDuration((long) options.pulseSingleDuration());
    setRepeatMode(ValueAnimator.RESTART);
    setRepeatCount(ValueAnimator.INFINITE);
    setInterpolator(interpolatorToUse);
  }

  start();
    end();
}