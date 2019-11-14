package com.mapbox.mapboxsdk.location;

import android.animation.FloatEvaluator;
import android.animation.TypeEvaluator;
import androidx.annotation.NonNull;

class MapboxFloatAnimator extends MapboxAnimator<Float> {
  MapboxFloatAnimator(Float previous, Float target, AnimationsValueChangeListener updateListener, int maxAnimationFps) {
    super(previous, target, updateListener, maxAnimationFps);
  }

  @NonNull
  @Override
  TypeEvaluator provideEvaluator() {
    return new FloatEvaluator();
  }
}
