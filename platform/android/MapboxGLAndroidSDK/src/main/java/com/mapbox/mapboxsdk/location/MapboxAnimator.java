package com.mapbox.mapboxsdk.location;

import android.animation.TypeEvaluator;
import android.animation.ValueAnimator;
import android.support.annotation.IntDef;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.util.List;

/**
 * Abstract class for all of the location component animators.
 *
 * @param <K> Data type that will be animated.
 * @param <L> Listener of animation updates.
 */
abstract class MapboxAnimator<K, L> extends ValueAnimator implements ValueAnimator.AnimatorUpdateListener {
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

  private final int animatorType = provideAnimatorType();
  final List<L> updateListeners;
  private final K target;

  MapboxAnimator(K previous, K target, List<L> updateListeners) {
    setObjectValues(previous, target);
    setEvaluator(provideEvaluator());
    this.updateListeners = updateListeners;
    this.target = target;
    addUpdateListener(this);
  }

  K getTarget() {
    return target;
  }

  @Type
  int getAnimatorType() {
    return animatorType;
  }

  @Type
  abstract int provideAnimatorType();

  abstract TypeEvaluator provideEvaluator();

  interface OnLayerAnimationsValuesChangeListener {
    void onNewLatLngValue(LatLng latLng);

    void onNewGpsBearingValue(float gpsBearing);

    void onNewCompassBearingValue(float compassBearing);

    void onNewAccuracyRadiusValue(float accuracyRadiusValue);
  }

  interface OnCameraAnimationsValuesChangeListener {
    void onNewLatLngValue(LatLng latLng);

    void onNewGpsBearingValue(float gpsBearing);

    void onNewCompassBearingValue(float compassBearing);

    void onNewZoomValue(float zoom);

    void onNewTiltValue(float tilt);
  }
}
