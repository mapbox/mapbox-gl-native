package com.mapbox.mapboxsdk.plugins.locationlayer;

import android.animation.ValueAnimator;

import java.util.List;

class CameraCompassBearingAnimator extends PluginFloatAnimator<PluginAnimator.OnCameraAnimationsValuesChangeListener> {
  CameraCompassBearingAnimator(Float previous, Float target,
                               List<OnCameraAnimationsValuesChangeListener> updateListeners) {
    super(previous, target, updateListeners);
  }

  @Override
  int provideAnimatorType() {
    return ANIMATOR_CAMERA_COMPASS_BEARING;
  }

  @Override
  public void onAnimationUpdate(ValueAnimator animation) {
    for (OnCameraAnimationsValuesChangeListener listener : updateListeners) {
      listener.onNewCompassBearingValue((Float) animation.getAnimatedValue());
    }
  }
}
