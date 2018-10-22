package com.mapbox.mapboxsdk.location;

import android.animation.ValueAnimator;

import java.util.List;

class CameraCompassBearingAnimator extends MapboxFloatAnimator<MapboxAnimator.OnCameraAnimationsValuesChangeListener> {
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
