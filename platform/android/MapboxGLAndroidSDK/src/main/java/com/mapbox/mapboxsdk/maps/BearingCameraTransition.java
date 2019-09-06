package com.mapbox.mapboxsdk.maps;

import android.view.animation.Interpolator;

public class BearingCameraTransition extends CameraTransition<Double> {

  public BearingCameraTransition(int type, long duration, long delay, Double endValue, Interpolator interpolator) {
    super(type, duration, delay, endValue, interpolator);
  }

  @Override
  int getCameraProperty() {
    return PROPERTY_BEARING;
  }

  @Override
  protected Double getAnimatedValue(double fraction) {
    return getStartValue() + ((getEndValue() - getStartValue()) * fraction);
  }

  public static class Builder extends CameraTransition.Builder<BearingCameraTransition, Double> {

    public Builder(Double endValue) {
      super(endValue);
    }

    @Override
    public BearingCameraTransition build() {
      return new BearingCameraTransition(reason, durationMillis, delayMillis, endValue, interpolator);
    }
  }
}
