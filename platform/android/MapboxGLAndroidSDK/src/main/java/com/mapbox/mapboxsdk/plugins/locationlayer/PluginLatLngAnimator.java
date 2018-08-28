package com.mapbox.mapboxsdk.plugins.locationlayer;

import android.animation.TypeEvaluator;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.List;

abstract class PluginLatLngAnimator<L> extends PluginAnimator<LatLng, L> {

  PluginLatLngAnimator(LatLng previous, LatLng target, List<L> updateListeners) {
    super(previous, target, updateListeners);
  }

  @Override
  TypeEvaluator provideEvaluator() {
    return new LatLngEvaluator();
  }
}
