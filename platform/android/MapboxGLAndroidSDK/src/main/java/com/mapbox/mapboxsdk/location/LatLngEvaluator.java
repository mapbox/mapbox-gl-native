package com.mapbox.mapboxsdk.location;

import android.animation.TypeEvaluator;

import com.mapbox.mapboxsdk.geometry.LatLng;

class LatLngEvaluator implements TypeEvaluator<LatLng> {

  private final LatLng latLng = new LatLng();

  @Override
  public LatLng evaluate(float fraction, LatLng startValue, LatLng endValue) {
    latLng.setLatitude(startValue.getLatitude()
      + ((endValue.getLatitude() - startValue.getLatitude()) * fraction));
    latLng.setLongitude(startValue.getLongitude()
      + ((endValue.getLongitude() - startValue.getLongitude()) * fraction));
    return latLng;
  }
}
