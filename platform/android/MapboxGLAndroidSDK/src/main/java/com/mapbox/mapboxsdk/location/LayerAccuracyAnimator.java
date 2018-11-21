package com.mapbox.mapboxsdk.location;

import android.animation.ValueAnimator;
import android.support.annotation.NonNull;

import java.util.List;

class LayerAccuracyAnimator extends MapboxFloatAnimator<MapboxAnimator.OnLayerAnimationsValuesChangeListener> {

  LayerAccuracyAnimator(Float previous, Float target, List<OnLayerAnimationsValuesChangeListener> updateListeners) {
    super(previous, target, updateListeners);
  }

  @Override
  int provideAnimatorType() {
    return ANIMATOR_LAYER_ACCURACY;
  }

  @Override
  public void onAnimationUpdate(@NonNull ValueAnimator animation) {
    for (OnLayerAnimationsValuesChangeListener listener : updateListeners) {
      listener.onNewAccuracyRadiusValue((Float) animation.getAnimatedValue());
    }
  }
}
