package com.mapbox.mapboxsdk.location;

import android.animation.ValueAnimator;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.List;

class CameraLatLngAnimator extends MapboxLatLngAnimator<MapboxAnimator.OnCameraAnimationsValuesChangeListener> {
  CameraLatLngAnimator(LatLng previous, LatLng target, List<OnCameraAnimationsValuesChangeListener> updateListeners) {
    super(previous, target, updateListeners);
  }

  @Override
  int provideAnimatorType() {
    return ANIMATOR_CAMERA_LATLNG;
  }

  @Override
  public void onAnimationUpdate(ValueAnimator animation) {
    for (OnCameraAnimationsValuesChangeListener listener : updateListeners) {
      listener.onNewLatLngValue((LatLng) animation.getAnimatedValue());
    }
  }
}
