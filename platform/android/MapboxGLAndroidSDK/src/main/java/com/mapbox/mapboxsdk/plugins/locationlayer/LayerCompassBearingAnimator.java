package com.mapbox.mapboxsdk.plugins.locationlayer;

import android.animation.ValueAnimator;

import java.util.List;

class LayerCompassBearingAnimator extends PluginFloatAnimator<PluginAnimator.OnLayerAnimationsValuesChangeListener> {
  LayerCompassBearingAnimator(Float previous,
                              Float target, List<OnLayerAnimationsValuesChangeListener> updateListeners) {
    super(previous, target, updateListeners);
  }

  @Override
  int provideAnimatorType() {
    return ANIMATOR_LAYER_COMPASS_BEARING;
  }

  @Override
  public void onAnimationUpdate(ValueAnimator animation) {
    for (OnLayerAnimationsValuesChangeListener listener : updateListeners) {
      listener.onNewCompassBearingValue((Float) animation.getAnimatedValue());
    }
  }
}
