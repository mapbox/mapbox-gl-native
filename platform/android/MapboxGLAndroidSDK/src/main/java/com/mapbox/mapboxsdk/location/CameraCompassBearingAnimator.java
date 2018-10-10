package com.mapbox.mapboxsdk.location;

import android.animation.ValueAnimator;
import android.support.annotation.NonNull;

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
  public void onAnimationUpdate(@NonNull ValueAnimator animation) {
    for (OnCameraAnimationsValuesChangeListener listener : updateListeners) {
      listener.onNewCompassBearingValue((Float) animation.getAnimatedValue());
    }
  }
}
