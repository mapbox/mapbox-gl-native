package com.mapbox.mapboxsdk.location;

import android.animation.ValueAnimator;
import android.support.annotation.NonNull;

import java.util.List;

class CameraGpsBearingAnimator extends MapboxFloatAnimator<MapboxAnimator.OnCameraAnimationsValuesChangeListener> {
  CameraGpsBearingAnimator(Float previous, Float target, List<OnCameraAnimationsValuesChangeListener> updateListeners) {
    super(previous, target, updateListeners);
  }

  @Override
  int provideAnimatorType() {
    return ANIMATOR_CAMERA_GPS_BEARING;
  }

  @Override
  public void onAnimationUpdate(@NonNull ValueAnimator animation) {
    for (OnCameraAnimationsValuesChangeListener listener : updateListeners) {
      listener.onNewGpsBearingValue((Float) animation.getAnimatedValue());
    }
  }
}
