package com.mapbox.mapboxsdk.maps;

import android.view.animation.Interpolator;

import com.mapbox.mapboxsdk.geometry.LatLng;

public class TargetCameraTransition extends CameraTransition<LatLng> {

  private final LatLng latLng = new LatLng();

  public TargetCameraTransition(int type, long delay, long duration, LatLng endValue, Interpolator interpolator) {
    super(type, duration, delay, endValue, interpolator);
  }

  @Override
  int getCameraProperty() {
    return PROPERTY_CENTER;
  }

  @Override
  LatLng getAnimatedValue(double fraction) {
    latLng.setLatitude(getStartValue().getLatitude()
      + ((getEndValue().getLatitude() - getStartValue().getLatitude()) * fraction));
    latLng.setLongitude(getStartValue().getLongitude()
      + ((getEndValue().getLongitude() - getStartValue().getLongitude()) * fraction));
    return latLng;
  }
}
