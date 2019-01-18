package com.mapbox.mapboxsdk.location;

import android.animation.ValueAnimator;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.util.Log;
import android.view.animation.Interpolator;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.Layer;

import static com.mapbox.mapboxsdk.location.LocationComponentConstants.PROPERTY_PULSING_CIRCLE_LAYER;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleOpacity;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleRadius;

/**
 * Manages the logic of the interpolated animation which is applied to the LocationComponent's pulsing circle
 */
class PulsingLocationCircleAnimator {

  private static final String TAG = "Mbgl-PulsingLocationCircleAnimator";
  private static final float PULSING_CIRCLE_RADIUS = 60;
  private Handler handler;
  private Runnable runnable;
  private float opacityCounter;

  public PulsingLocationCircleAnimator() {
    this.opacityCounter = 0;
  }

  /**
   * Start the LocationComponent circle pulse animation
   *
   * @param interpolatorToUse   the type of Android-system interpolator to use
   * @param mapboxMap           the MapboxMap object which pulsing circle should be shown on
   * @param locationComponentOptions the stying options of the LocationComponent pulsing circle
   */
  public void animatePulsingCircleRadius(@NonNull final Interpolator interpolatorToUse,
                                         @NonNull final MapboxMap mapboxMap,
                                         @NonNull final LocationComponentOptions locationComponentOptions) {
    handler = new Handler();
    runnable = new Runnable() {
      @Override
      public void run() {
        // Check if we are at the end of the points list, if so we want to stop using
        // the handler.
        if (mapboxMap.getStyle().getLayer(PROPERTY_PULSING_CIRCLE_LAYER) != null) {

          opacityCounter = 0;

          final Layer pulsingCircleLayer = mapboxMap.getStyle().getLayer(PROPERTY_PULSING_CIRCLE_LAYER);
          ValueAnimator animator = ValueAnimator.ofFloat(0f, PULSING_CIRCLE_RADIUS);
          animator.setDuration((long) locationComponentOptions.pulseSingleDuration());
          animator.setRepeatMode(ValueAnimator.RESTART);
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
          animator.start();
          // Once we finish we need to repeat the entire process by executing the
          // handler again once the ValueAnimator is finished.
          handler.postDelayed(this, (long) locationComponentOptions.pulseFrequency());
        }
      }
    };
    handler.post(runnable);
  }

  public void stopPulsingAnimation() {
    Log.d(TAG, "stopPulsingAnimation: ");
    handler.removeCallbacks(runnable);
  }
}
