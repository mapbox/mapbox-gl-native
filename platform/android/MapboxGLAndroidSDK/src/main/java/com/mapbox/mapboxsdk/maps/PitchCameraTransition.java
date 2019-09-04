package com.mapbox.mapboxsdk.maps;

import android.view.animation.Interpolator;

public class PitchCameraTransition extends CameraTransition<Double> {

  public PitchCameraTransition(int type, double duration, double delay, Double endValue, Interpolator interpolator) {
    super(type, duration, delay, endValue, interpolator);
  }

  @Override
  int getCameraProperty() {
    return PROPERTY_PITCH;
  }

  @Override
  Double getAnimatedValue(double fraction) {
    return getStartValue() + ((getEndValue() - getStartValue()) * fraction);
  }
}
