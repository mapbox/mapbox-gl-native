package com.mapbox.mapboxsdk.location;

import android.animation.TypeEvaluator;

import android.support.annotation.NonNull;
import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.List;

abstract class MapboxLatLngAnimator<L> extends MapboxAnimator<LatLng, L> {

  MapboxLatLngAnimator(LatLng previous, LatLng target, List<L> updateListeners) {
    super(previous, target, updateListeners);
  }

  @NonNull
  @Override
  TypeEvaluator provideEvaluator() {
    return new LatLngEvaluator();
  }
}
