package com.mapbox.mapboxsdk.location;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.TypeEvaluator;
import android.animation.ValueAnimator;
import androidx.annotation.IntDef;
import androidx.annotation.NonNull;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * Abstract class for all of the location component animators.
 *
 * @param <K> Data type that will be animated.
 */
abstract class MapboxAnimator<K> extends ValueAnimator implements ValueAnimator.AnimatorUpdateListener {
  @Retention(RetentionPolicy.SOURCE)
  @IntDef( {
    ANIMATOR_LAYER_LATLNG,
    ANIMATOR_CAMERA_LATLNG,
    ANIMATOR_LAYER_GPS_BEARING,
    ANIMATOR_LAYER_COMPASS_BEARING,
    ANIMATOR_CAMERA_GPS_BEARING,
    ANIMATOR_CAMERA_COMPASS_BEARING,
    ANIMATOR_LAYER_ACCURACY,
    ANIMATOR_ZOOM,
    ANIMATOR_TILT
  })
  @interface Type {
  }

  static final int ANIMATOR_LAYER_LATLNG = 0;
  static final int ANIMATOR_CAMERA_LATLNG = 1;
  static final int ANIMATOR_LAYER_GPS_BEARING = 2;
  static final int ANIMATOR_LAYER_COMPASS_BEARING = 3;
  static final int ANIMATOR_CAMERA_GPS_BEARING = 4;
  static final int ANIMATOR_CAMERA_COMPASS_BEARING = 5;
  static final int ANIMATOR_LAYER_ACCURACY = 6;
  static final int ANIMATOR_ZOOM = 7;
  static final int ANIMATOR_TILT = 8;

  private final AnimationsValueChangeListener<K> updateListener;
  private final K target;
  private K animatedValue;

  private final double minUpdateInterval;
  private long timeElapsed;

  MapboxAnimator(@NonNull K previous, @NonNull K target, @NonNull AnimationsValueChangeListener<K> updateListener,
                 int maxAnimationFps) {
    minUpdateInterval = 1E9 / maxAnimationFps;
    setObjectValues(previous, target);
    setEvaluator(provideEvaluator());
    this.updateListener = updateListener;
    this.target = target;
    addUpdateListener(this);
    addListener(new AnimatorListener());
  }

  @Override
  public void onAnimationUpdate(ValueAnimator animation) {
    animatedValue = (K) animation.getAnimatedValue();

    long currentTime = System.nanoTime();
    if ((currentTime - timeElapsed) < minUpdateInterval) {
      return;
    }

    postUpdates();
    timeElapsed = currentTime;
  }

  private class AnimatorListener extends AnimatorListenerAdapter {
    @Override
    public void onAnimationEnd(Animator animation) {
      postUpdates();
    }
  }

  private void postUpdates() {
    updateListener.onNewAnimationValue(animatedValue);
  }

  K getTarget() {
    return target;
  }

  abstract TypeEvaluator provideEvaluator();

  interface AnimationsValueChangeListener<K> {
    void onNewAnimationValue(K value);
  }
}
