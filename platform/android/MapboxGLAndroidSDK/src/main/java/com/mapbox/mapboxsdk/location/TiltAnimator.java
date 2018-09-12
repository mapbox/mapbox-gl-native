package com.mapbox.mapboxsdk.location;

import android.animation.ValueAnimator;
import android.support.annotation.Nullable;

import com.mapbox.mapboxsdk.maps.MapboxMap;

import java.util.List;

class TiltAnimator extends MapboxCameraAnimatorAdapter {
  TiltAnimator(Float previous, Float target, List<OnCameraAnimationsValuesChangeListener> updateListeners,
               @Nullable MapboxMap.CancelableCallback cancelableCallback) {
    super(previous, target, updateListeners, cancelableCallback);
  }

  @Override
  int provideAnimatorType() {
    return ANIMATOR_TILT;
  }

  @Override
  public void onAnimationUpdate(ValueAnimator animation) {
    for (OnCameraAnimationsValuesChangeListener listener : updateListeners) {
      listener.onNewTiltValue((Float) animation.getAnimatedValue());
    }
  }
}
