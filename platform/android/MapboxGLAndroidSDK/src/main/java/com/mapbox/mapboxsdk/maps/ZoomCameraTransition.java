package com.mapbox.mapboxsdk.maps;

import android.view.animation.Interpolator;

public class ZoomCameraTransition extends CameraTransition<Double> {

  public ZoomCameraTransition(int type, double duration, double delay, Double endValue, Interpolator interpolator) {
    super(type, duration, delay, endValue, interpolator);
  }

  @Override
  int getCameraProperty() {
    return PROPERTY_ZOOM;
  }

  @Override
  Double getAnimatedValue(double fraction) {
    return getStartValue() + ((getEndValue() - getStartValue()) * fraction);
  }
}
