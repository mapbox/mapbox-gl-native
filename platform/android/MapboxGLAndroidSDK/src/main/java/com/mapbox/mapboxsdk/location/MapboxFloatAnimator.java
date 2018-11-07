package com.mapbox.mapboxsdk.location;

import android.animation.FloatEvaluator;
import android.animation.TypeEvaluator;
import android.support.annotation.NonNull;

import java.util.List;

abstract class MapboxFloatAnimator<L> extends MapboxAnimator<Float, L> {
  MapboxFloatAnimator(Float previous, Float target, List<L> updateListeners) {
    super(previous, target, updateListeners);
  }

  @NonNull
  @Override
  TypeEvaluator provideEvaluator() {
    return new FloatEvaluator();
  }
}
