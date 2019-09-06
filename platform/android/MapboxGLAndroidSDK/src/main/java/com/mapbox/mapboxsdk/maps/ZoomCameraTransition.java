package com.mapbox.mapboxsdk.maps;

import android.view.animation.Interpolator;

public class ZoomCameraTransition extends CameraTransition<Double> {

  public ZoomCameraTransition(int type, long duration, long delay, Double endValue, Interpolator interpolator) {
    super(type, duration, delay, endValue, interpolator);
  }

  @Override
  int getCameraProperty() {
    return PROPERTY_ZOOM;
  }

  @Override
  protected Double getAnimatedValue(double fraction) {
    return getStartValue() + ((getEndValue() - getStartValue()) * fraction);
  }

  public static class Builder extends CameraTransition.Builder<ZoomCameraTransition, Double> {

    public Builder(Double endValue) {
      super(endValue);
    }

    @Override
    public ZoomCameraTransition build() {
      return new ZoomCameraTransition(reason, durationMillis, delayMillis, endValue, interpolator);
    }
  }
}
