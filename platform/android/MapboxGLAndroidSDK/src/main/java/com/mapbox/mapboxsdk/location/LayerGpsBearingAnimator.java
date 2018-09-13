package com.mapbox.mapboxsdk.location;

import android.animation.ValueAnimator;

import java.util.List;

class LayerGpsBearingAnimator extends MapboxFloatAnimator<MapboxAnimator.OnLayerAnimationsValuesChangeListener> {
  LayerGpsBearingAnimator(Float previous, Float target, List<OnLayerAnimationsValuesChangeListener> updateListeners) {
    super(previous, target, updateListeners);
  }

  @Override
  int provideAnimatorType() {
    return ANIMATOR_LAYER_GPS_BEARING;
  }

  @Override
  public void onAnimationUpdate(ValueAnimator animation) {
    for (OnLayerAnimationsValuesChangeListener listener : updateListeners) {
      listener.onNewGpsBearingValue((Float) animation.getAnimatedValue());
    }
  }
}
