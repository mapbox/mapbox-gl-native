package com.mapbox.mapboxsdk.location;

import android.animation.ValueAnimator;
import android.support.annotation.NonNull;

import java.util.List;

class LayerCompassBearingAnimator extends MapboxFloatAnimator<MapboxAnimator.OnLayerAnimationsValuesChangeListener> {
  LayerCompassBearingAnimator(Float previous,
                              Float target, List<OnLayerAnimationsValuesChangeListener> updateListeners) {
    super(previous, target, updateListeners);
  }

  @Override
  int provideAnimatorType() {
    return ANIMATOR_LAYER_COMPASS_BEARING;
  }

  @Override
  public void onAnimationUpdate(@NonNull ValueAnimator animation) {
    for (OnLayerAnimationsValuesChangeListener listener : updateListeners) {
      listener.onNewCompassBearingValue((Float) animation.getAnimatedValue());
    }
  }
}
