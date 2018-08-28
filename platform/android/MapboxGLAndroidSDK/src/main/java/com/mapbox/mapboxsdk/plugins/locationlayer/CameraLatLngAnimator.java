package com.mapbox.mapboxsdk.plugins.locationlayer;

import android.animation.ValueAnimator;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.List;

class CameraLatLngAnimator extends PluginLatLngAnimator<PluginAnimator.OnCameraAnimationsValuesChangeListener> {
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
