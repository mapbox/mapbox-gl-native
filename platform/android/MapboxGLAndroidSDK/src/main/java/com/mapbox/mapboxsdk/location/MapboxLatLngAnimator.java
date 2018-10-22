package com.mapbox.mapboxsdk.location;

import android.animation.TypeEvaluator;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.List;

abstract class MapboxLatLngAnimator<L> extends MapboxAnimator<LatLng, L> {

  MapboxLatLngAnimator(LatLng previous, LatLng target, List<L> updateListeners) {
    super(previous, target, updateListeners);
  }

  @Override
  TypeEvaluator provideEvaluator() {
    return new LatLngEvaluator();
  }
}
