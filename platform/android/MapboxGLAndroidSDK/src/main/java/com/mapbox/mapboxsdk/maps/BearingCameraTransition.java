package com.mapbox.mapboxsdk.maps;

import android.view.animation.Interpolator;

public class BearingCameraTransition extends CameraTransition<Double> {

  public BearingCameraTransition(int type, double duration, double delay, Double endValue, Interpolator interpolator) {
    super(type, duration, delay, endValue, interpolator);
  }

  @Override
  int getCameraProperty() {
    return PROPERTY_BEARING;
  }

  @Override
  Double getAnimatedValue(double fraction) {
    return getStartValue() + ((getEndValue() - getStartValue()) * fraction);
  }
}
