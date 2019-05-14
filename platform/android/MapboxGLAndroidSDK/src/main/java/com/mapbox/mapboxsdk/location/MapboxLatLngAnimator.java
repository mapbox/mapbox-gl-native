package com.mapbox.mapboxsdk.location;

import android.animation.TypeEvaluator;
import androidx.annotation.NonNull;

import com.mapbox.mapboxsdk.geometry.LatLng;

class MapboxLatLngAnimator extends MapboxAnimator<LatLng> {

  MapboxLatLngAnimator(LatLng previous, LatLng target, AnimationsValueChangeListener updateListener,
                       int maxAnimationFps) {
    super(previous, target, updateListener, maxAnimationFps);
  }

  @NonNull
  @Override
  TypeEvaluator provideEvaluator() {
    return new LatLngEvaluator();
  }
}
