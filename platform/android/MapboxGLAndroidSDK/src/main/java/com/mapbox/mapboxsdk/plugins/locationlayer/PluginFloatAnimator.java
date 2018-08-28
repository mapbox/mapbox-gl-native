package com.mapbox.mapboxsdk.plugins.locationlayer;

import android.animation.FloatEvaluator;
import android.animation.TypeEvaluator;

import java.util.List;

abstract class PluginFloatAnimator<L> extends PluginAnimator<Float, L> {
  PluginFloatAnimator(Float previous, Float target, List<L> updateListeners) {
    super(previous, target, updateListeners);
  }

  @Override
  TypeEvaluator provideEvaluator() {
    return new FloatEvaluator();
  }
}
