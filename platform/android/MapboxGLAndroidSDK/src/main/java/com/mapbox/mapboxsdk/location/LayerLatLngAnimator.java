package com.mapbox.mapboxsdk.location;

import android.animation.ValueAnimator;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.List;

class LayerLatLngAnimator extends MapboxLatLngAnimator<MapboxAnimator.OnLayerAnimationsValuesChangeListener> {
  LayerLatLngAnimator(LatLng previous, LatLng target, List<OnLayerAnimationsValuesChangeListener> updateListeners) {
    super(previous, target, updateListeners);
  }

  @Override
  int provideAnimatorType() {
    return ANIMATOR_LAYER_LATLNG;
  }

  @Override
  public void onAnimationUpdate(ValueAnimator animation) {
    for (OnLayerAnimationsValuesChangeListener listener : updateListeners) {
      listener.onNewLatLngValue((LatLng) animation.getAnimatedValue());
    }
  }
}
