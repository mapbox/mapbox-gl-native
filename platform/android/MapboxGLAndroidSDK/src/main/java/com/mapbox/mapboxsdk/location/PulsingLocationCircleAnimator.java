package com.mapbox.mapboxsdk.location;

import android.animation.TypeEvaluator;
import android.animation.ValueAnimator;
import android.os.Build;
import android.support.annotation.NonNull;
import android.util.Log;
import android.view.animation.Interpolator;

import com.mapbox.mapboxsdk.log.Logger;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.Layer;

import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_PULSING_CIRCLE_LAYER;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleOpacity;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleRadius;

/**
 * Manages the logic of the interpolated animation which is applied to the LocationComponent's pulsing circle
 */
public class PulsingLocationCircleAnimator extends MapboxAnimator  {

  private static final String TAG = "Mbgl-PulsingLocationCircleAnimator";
  private static final float PULSING_CIRCLE_RADIUS = 60;
  private float opacityCounter;
  private Interpolator interpolatorToUse;
  private MapboxMap mapboxMap;
  private LocationComponentOptions locationComponentOptions;
  private Layer pulsingCircleLayer;
  private ValueAnimator animator;

  /**
   * Start the LocationComponent circle pulse animation
   *
   * @param interpolatorToUse        the type of Android-system interpolator to use
   * @param mapboxMap                the MapboxMap object which pulsing circle should be shown on
   * @param locationComponentOptions the stying options of the LocationComponent pulsing circle
   */
  public PulsingLocationCircleAnimator(@NonNull final Interpolator interpolatorToUse,
                                       @NonNull final MapboxMap mapboxMap,
                                       @NonNull final LocationComponentOptions locationComponentOptions) {
    Logger.d(TAG,"creating PulsingLocationCircleAnimator()");
    this.opacityCounter = 0;
    this.interpolatorToUse = interpolatorToUse;
    this.mapboxMap = mapboxMap;
    this.locationComponentOptions = locationComponentOptions;
  }

  @Override
  public void onAnimationUpdate(ValueAnimator animation) {
    super.onAnimationUpdate(animation);
  }

  @Override
  Object getTarget() {
    return super.getTarget();
  }

  @Override
  TypeEvaluator provideEvaluator() {
    return null;
  }

  public void startPulsingAnimation() {
    createValueAnimator();
    startValueAnimator();
  }

  private void createValueAnimator() {
    if (mapboxMap.getStyle().getLayer(PROPERTY_PULSING_CIRCLE_LAYER) != null) {
      Logger.d(TAG, "mapboxMap.getStyle().getLayer(PROPERTY_PULSING_CIRCLE_LAYER) != null");
      Logger.d(TAG, "locationComponentOptions.pulseSingleDuration() = " + locationComponentOptions.pulseSingleDuration());
      pulsingCircleLayer = mapboxMap.getStyle().getLayer(PROPERTY_PULSING_CIRCLE_LAYER);
      animator = ValueAnimator.ofFloat(0f, PULSING_CIRCLE_RADIUS);
      animator.setDuration((long) locationComponentOptions.pulseSingleDuration());
      animator.setRepeatMode(ValueAnimator.RESTART);
      animator.setRepeatCount(ValueAnimator.INFINITE);
      animator.setInterpolator(interpolatorToUse);
      animator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
        @Override
        public void onAnimationUpdate(ValueAnimator valueAnimator) {
          pulsingCircleLayer.setProperties(circleRadius((Float) valueAnimator.getAnimatedValue()));
          if (locationComponentOptions.pulsingCircleFadeEnabled()) {
            pulsingCircleLayer.setProperties(circleOpacity(1 - opacityCounter * .01f));
            opacityCounter++;
          }
        }
      });
    }
  }

  private void startValueAnimator() {
    if (animator != null) {
      Log.d(TAG, "startValueAnimator() animator != null");
      animator.start();
    }
  }

  public boolean pulsingAnimationIsStarted() {
    if (animator != null) {
      return animator.isStarted();
    }
    return false;
  }

  public boolean pulsingAnimationIsRunning() {
    if (animator != null) {
      return animator.isRunning();
    }
    return false;
  }

  public void pausePulsingAnimation() {
    if (animator != null && Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
      animator.pause();
    } else {
      animator.end();
    }
  }

  public void resumePulsingAnimation() {
    if (animator != null && Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
      animator.resume();
//      createValueAnimator();
//      startValueAnimator();
    }
  }

  public void stopPulsingAnimation() {
    if (animator != null) {
      animator.end();
    }
  }
}