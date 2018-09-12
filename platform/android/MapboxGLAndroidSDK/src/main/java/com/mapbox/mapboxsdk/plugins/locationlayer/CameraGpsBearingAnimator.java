package com.mapbox.mapboxsdk.plugins.locationlayer;

import android.animation.ValueAnimator;

import java.util.List;

class CameraGpsBearingAnimator extends PluginFloatAnimator<PluginAnimator.OnCameraAnimationsValuesChangeListener> {
  CameraGpsBearingAnimator(Float previous, Float target, List<OnCameraAnimationsValuesChangeListener> updateListeners) {
    super(previous, target, updateListeners);
  }

  @Override
  int provideAnimatorType() {
    return ANIMATOR_CAMERA_GPS_BEARING;
  }

  @Override
  public void onAnimationUpdate(ValueAnimator animation) {
    for (OnCameraAnimationsValuesChangeListener listener : updateListeners) {
      listener.onNewGpsBearingValue((Float) animation.getAnimatedValue());
    }
  }
}
